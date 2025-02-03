#include "BushActor.h"

#include "ALightTankCharacter.h"
#include "FastLogger.h"
#include "MyPawn.h"
#include "NavigationSystemTypes.h"
#include "Components/SphereComponent.h"
#include "MyProject/Mk/Character/Public/Mk_TankPawn.h"

ABushActor::ABushActor()
{
	SphereRoot = CreateDefaultSubobject<USphereComponent>(TEXT("SphereRoot"));
	SetRootComponent(SphereRoot);
	SphereRoot->SetSphereRadius(1500.f); // 1500cm = 15m
	SphereRoot->SetCollisionObjectType(ECC_GameTraceChannel7);
	SphereRoot->SetCollisionResponseToAllChannels(ECR_Ignore);
	// SphereRoot->SetCollisionRespons eToChannel(ECC_GameTraceChannel7, ECR_Block); // Bush for LineTrace
	SphereRoot->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap); // Enemy Tank
	SphereRoot->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap); // Player Tank

	BushCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BushCollision"));
	BushCollision->SetupAttachment(SphereRoot);
	BushCollision->SetCollisionObjectType(ECC_GameTraceChannel7);
	BushCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BushCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Block);
	
	BushMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BushMesh"));
	BushMesh->SetupAttachment(SphereRoot);
	BushMesh->SetRelativeRotation({180.0f, 0.0f, 0.0f});
	BushMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BushMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Bush
	(TEXT("/Game/Fab/Green_Bush/green_bush.green_bush"));
	if (SM_Bush.Succeeded())
	{
		BushMesh->SetStaticMesh(SM_Bush.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_OriginalBark
	(TEXT("/Game/Fab/Green_Bush/Bark.Bark"));
	if (MI_OriginalBark.Succeeded())
	{
		OriginalBark = MI_OriginalBark.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_OriginalLeaf
	(TEXT("/Game/Fab/Green_Bush/Leaf.Leaf"));
	if (MI_OriginalLeaf.Succeeded())
	{
		OriginalLeaf = MI_OriginalLeaf.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_TransparencyBark
	(TEXT("/Game/Fab/Green_Bush/M_Bark_Trans.M_Bark_Trans"));
	if (MI_TransparencyBark.Succeeded())
	{
		TransparencyBark = MI_TransparencyBark.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_TransparencyLeaf
	(TEXT("/Game/Fab/Green_Bush/M_Leaf_Trans.M_Leaf_Trans"));
	if (MI_TransparencyLeaf.Succeeded())
	{
		TransparencyLeaf = MI_TransparencyLeaf.Object;
	}

	SphereRoot->OnComponentBeginOverlap.AddDynamic(this, &ABushActor::OnBeginOverlap);
	SphereRoot->OnComponentEndOverlap.AddDynamic(this, &ABushActor::OnEndOverlap);
}

void ABushActor::BeginPlay()
{
	Super::BeginPlay();

	// 모든 탱크의 공격 Delegate에 함수를 등록
	// 게임 모드에서 함수를 호출하는 것으로 해주자. 생각해보니 주기적으로 생성됨
}

void ABushActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Type::Destroyed)
	{
		GetWorld()->GetTimerManager().ClearTimer(CoverHandle);
	}
}

void ABushActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Is Locally Controlled
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (PlayerPawn && PlayerPawn->IsLocallyControlled())
	{
		bOverlap = true;
		BushMesh->SetMaterial(0, TransparencyBark);
		BushMesh->SetMaterial(1, TransparencyLeaf);
		// 충돌도 일시적으로 멈출 필요가 있음
		BushCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	}

	// 공격시 투명화 되는 Delegate를 호출
	ALightTankCharacter* LightTank = Cast<ALightTankCharacter>(OtherActor);
	if (LightTank)
	{
		LightTank->GetAttackDelegate().OnAttackDelegate.AddDynamic(this, &ABushActor::OnAttackDetected);
	}
	AMyPawn* SkyTank = Cast<AMyPawn>(OtherActor);
	if (SkyTank)
	{
		SkyTank->GetAttackDelegate().OnAttackDelegate.AddDynamic(this, &ABushActor::OnAttackDetected);
	}
	AMk_TankPawn* MkTank = Cast<AMk_TankPawn>(OtherActor);
	if (MkTank)
	{
		MkTank->GetAttackDelegate().OnAttackDelegate.AddDynamic(this, &ABushActor::OnAttackDetected);
	}
}

void ABushActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Is Locally Controlled
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (PlayerPawn && PlayerPawn->IsLocallyControlled())
	{
		bOverlap = false;
		BushMesh->SetMaterial(0, OriginalBark);
		BushMesh->SetMaterial(1, OriginalLeaf);
		// 충돌을 다시 넣어야 함
		BushCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Block);
	}

	ALightTankCharacter* LightTank = Cast<ALightTankCharacter>(OtherActor);
	if (LightTank)
	{
		LightTank->GetAttackDelegate().OnAttackDelegate.RemoveDynamic(this, &ABushActor::OnAttackDetected);
	}
	AMyPawn* SkyTank = Cast<AMyPawn>(OtherActor);
	if (SkyTank)
	{
		SkyTank->GetAttackDelegate().OnAttackDelegate.RemoveDynamic(this, &ABushActor::OnAttackDetected);
	}
	AMk_TankPawn* MkTank = Cast<AMk_TankPawn>(OtherActor);
	if (MkTank)
	{
		MkTank->GetAttackDelegate().OnAttackDelegate.RemoveDynamic(this, &ABushActor::OnAttackDetected);
	}
}

void ABushActor::OnAttackDetected()
{
	bCoverable = false;

	BushMesh->SetMaterial(0, TransparencyBark);
	BushMesh->SetMaterial(1, TransparencyLeaf);
	
	GetWorld()->GetTimerManager().ClearTimer(CoverHandle);
	GetWorld()->GetTimerManager().SetTimer(CoverHandle, this, &ABushActor::ReCover, 3.0f, false);
}

void ABushActor::ReCover()
{
	bCoverable = true;

	if (bOverlap)
	{
		return ;
	}
	BushMesh->SetMaterial(0, OriginalBark);
	BushMesh->SetMaterial(1, OriginalLeaf);
}

void ABushActor::BindAttackDelegate(FAttackDelegate& AttackDelegate)
{
	AttackDelegate.OnAttackDelegate.AddDynamic(this, &ABushActor::OnAttackDetected);
}
