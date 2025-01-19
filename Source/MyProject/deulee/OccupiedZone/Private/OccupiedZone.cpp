#include "OccupiedZone.h"

#include "DamageInterface.h"
#include "FastLogger.h"
#include "TeamInterface.h"
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
}

void AOccupiedZone::BeginPlay()
{
	Super::BeginPlay();
}

void AOccupiedZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
	ETeam PrevTeam = ResultTeam;
	for (int i = 0; i < OverlappedTanks.Num(); ++i)
	{
		if (!OverlappedTanks[i]->GetClass()->ImplementsInterface(UTeamInterface::StaticClass()))
		{
			continue;
		}
		ResultTeam = ITeamInterface::Execute_GetTeam(OverlappedTanks[i]);
		if (PrevTeam == ETeam::None)
		{
			PrevTeam = ResultTeam;
		}
		else if (PrevTeam != ResultTeam)
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
	ControllingTeam = DetermineControllingTeam();
	ControllingTeam = ValidateTanksState() ? ControllingTeam : ETeam::None;
}

void AOccupiedZone::BroadCaseWinningTeam() const
{
	// -1 : AI, 1 : Player
	if (FMath::Abs(CaptureProgress) >= 100)
	{
		OnWinnerDetectedDelegate.Broadcast(ControllingTeam);
	}
}

void AOccupiedZone::UpdateCaptureProgress(float DeltaSeconds)
{
	switch (ControllingTeam)
	{
	case ETeam::Player:
		CaptureProgress += CaptureRate * DeltaSeconds;
		break;
	case ETeam::AI:
		CaptureProgress -= CaptureRate * DeltaSeconds;
		break;
	default:
		break;
	}
	CaptureProgress = FMath::Clamp(CaptureProgress, -100.f, 100.f);
}
