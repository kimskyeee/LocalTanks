#include "OccupiedZone.h"

#include "DamageInterface.h"
#include "FastLogger.h"
#include "OccupyUI.h"
#include "TeamInterface.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"

AOccupiedZone::AOccupiedZone()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereCollision);

	CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh"));
	CylinderMesh->SetupAttachment(SphereCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_CylinderMeshAsset
	(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (SM_CylinderMeshAsset.Succeeded())
	{
		CylinderMesh->SetStaticMesh(SM_CylinderMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_CylinderMeshAsset
	(TEXT("/Game/Material/OccupiedZone/MI_OccupiedZone.MI_OccupiedZone"));
	if (MI_CylinderMeshAsset.Succeeded())
	{
		CylinderMesh->SetMaterial(0, MI_CylinderMeshAsset.Object);
	}

	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap); // Enemy Tank
	SphereCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap); // Player Tank

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AOccupiedZone::OnTankBeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AOccupiedZone::OnTankEndOverlap);

	CylinderMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	CylinderMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	static ConstructorHelpers::FClassFinder<UOccupyUI> WBP_OccupyUIClass
	(TEXT("/Game/NewSkye/GameUI/WBP_Occupy.WBP_Occupy_C"));
	if (WBP_OccupyUIClass.Succeeded())
	{
		OccupyUIClass = WBP_OccupyUIClass.Class;
	}
}

void AOccupiedZone::BeginPlay()
{
	Super::BeginPlay();

	OccupyUI = Cast<UOccupyUI>(CreateWidget<UOccupyUI>(GetWorld(), OccupyUIClass));
	if (OccupyUI)
	{
		OccupyUI->AddToViewport();
	}
}

void AOccupiedZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bGameEnd)
	{
		return;
	}
	SetControllingTeam();
	UpdateCaptureProgress(DeltaSeconds);
	BroadCaseWinningTeam();
}

void AOccupiedZone::OnTankBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
                                       class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedTanks.Contains(OtherActor))
	{
		return;
	}
	OverlappedTanks.Add(OtherActor);
	OnTankBeginOverlapDelegate.Broadcast();
}

void AOccupiedZone::OnTankEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OverlappedTanks.Contains(OtherActor))
	{
		return;
	}
	OverlappedTanks.Remove(OtherActor);
	OnTankEndOverlapDelegate.Broadcast();
}

ETeam AOccupiedZone::DetermineControllingTeam()
{
	ETeam ResultTeam = ETeam::None;
	ETeam InPrevTeam = ResultTeam;
	for (int i = 0; i < OverlappedTanks.Num(); ++i)
	{
		if (!OverlappedTanks[i]->GetClass()->ImplementsInterface(UTeamInterface::StaticClass()))
		{
			continue;
		}
		ResultTeam = ITeamInterface::Execute_GetTeam(OverlappedTanks[i]);
		if (InPrevTeam == ETeam::None)
		{
			InPrevTeam = ResultTeam;
		}
		else if (InPrevTeam != ResultTeam)
		{
			return ETeam::None;
		}
	}
	return ResultTeam;
}

bool AOccupiedZone::ValidateTanksState()
{
	bool bFlag = true;

	AActor* PlayerTank = GetWorld()->GetFirstPlayerController()->GetPawn();
	for (int i = 0; i < OverlappedTanks.Num(); ++i)
	{
		if (!OverlappedTanks[i]->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
		{
			continue;
		}

		if (!OverlappedTanks[i]->GetClass()->ImplementsInterface(UTeamInterface::StaticClass()))
		{
			continue;
		}

		// 팀이 다르면 검사 안함
		ETeam TankTeam = ITeamInterface::Execute_GetTeam(OverlappedTanks[i]);
		if (TankTeam != ControllingTeam)
		{
			continue;
		}

		// 해당 탱크 중 하나라도 점령이 불가능 하면 false 반환
		bool bCanOccupy = IDamageInterface::Execute_IsPossibleToOccpupy(OverlappedTanks[i]);
		bFlag &= bCanOccupy;
	}
	return bFlag;
}

void AOccupiedZone::SetControllingTeam()
{
	// 팀 결정 로직 매핑
	PrevTeam = ControllingTeam == ETeam::None ? PrevTeam : ControllingTeam;
	ControllingTeam = DetermineControllingTeam();
	ControllingTeam = ValidateTanksState() ? ControllingTeam : ETeam::None;
}

void AOccupiedZone::BroadCaseWinningTeam()
{
	// -1 : AI, 1 : Player
	if (FMath::Abs(CaptureProgress) >= 100)
	{
		OnWinnerDetectedDelegate.Broadcast(ControllingTeam);
		OccupyUI->OProgressReset();
		bGameEnd = true;
	}
}

void AOccupiedZone::UpdateCaptureProgress(float DeltaSeconds)
{
	if (ControllingTeam != PrevTeam && ControllingTeam != ETeam::None)
	{
		CaptureProgress = 0;
		OccupyUI->OProgressReset();
	}
	
	int32 Sign = ControllingTeam == ETeam::Player ? 1 : ControllingTeam == ETeam::None ? 0 : -1;
	CaptureProgress += CaptureRate * DeltaSeconds * Sign;
	CaptureProgress = FMath::Clamp(CaptureProgress, -100.f, 100.f);

	if (Sign == 0)
	{
		// 천천히 줄어듬 (절대값 기준으로 0이 될 때까지)
		CaptureProgress += CaptureRate * DeltaSeconds * FMath::Sign(CaptureProgress) * -1;
		CaptureProgress = FMath::Abs(CaptureProgress) < 0.1f ? 0 : CaptureProgress;
	}

	constexpr float MaxCaptureProgress = 100;

	switch (Sign)
	{
	case 1:
		OccupyUI->OProgressBarGreen(CaptureProgress, MaxCaptureProgress);
		break;
	case -1:
		OccupyUI->OProgressBarRed(FMath::Abs(CaptureProgress), MaxCaptureProgress);
		break;
	default:
		// 현재 점령 중인 팀이 없는 경우 이전의 팀에 따라서 색을 칠함
		if (PrevTeam == ETeam::Player)
		{
			OccupyUI->OProgressBarGreen(CaptureProgress, MaxCaptureProgress);
		}
		else if (PrevTeam == ETeam::AI)
		{
			OccupyUI->OProgressBarRed(FMath::Abs(CaptureProgress), MaxCaptureProgress);
		}
		break;
	}
}
