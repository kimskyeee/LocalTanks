#include "AShell.h"

#include "FastLogger.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "ShellDecalComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AShell::AShell()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->SetMobility(EComponentMobility::Movable);
	Collision->SetBoxExtent({30, 30, 30});

	BulletNS = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletNS"));
	BulletNS->SetupAttachment(Collision);
	BulletNS->SetMobility(EComponentMobility::Movable);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_Bullet
	(TEXT("/Game/LightTank/Character/NS_bullet.NS_bullet"));
	if (NS_Bullet.Succeeded())
	{
		BulletNS->SetAsset(NS_Bullet.Object);
	}
	BulletNS->SetCollisionResponseToChannels(ECR_Ignore);
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ShellDecalComponent = CreateDefaultSubobject<UShellDecalComponent>(TEXT("ShellDecalComponent"));

	const ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_Explosion
	(TEXT("/Game/MsvFx_Niagara_Explosion_Pack_01/Prefabs/Niagara_Explosion_02.Niagara_Explosion_02"));
	if (NS_Explosion.Succeeded())
	{
		ExplosionNSAsset = NS_Explosion.Object;
	}
}

void AShell::BeginPlay()
{
	Super::BeginPlay();

	// DeActive();
}

void AShell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bActive)
	{
		return ;
	}
}

void AShell::SetShellInfo(FShellBasicInfo ShellInfo_)
{
	this->ShellInfo = ShellInfo_;
}

FShellBasicInfo AShell::GetShellInfo()
{
	return ShellInfo;
}

void AShell::SetArmor(UArmor* Armor_)
{
	this->Armor = Armor_;
}

void AShell::DeActive()
{
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
	}
	SetActorLocation(FVector::ZeroVector);
	// SetActorHiddenInGame(true);
	BulletNS->SetVisibility(false);
	bActive = false;
}

void AShell::SetCollisionPreset(FName ProfileName)
{
	Collision->SetCollisionProfileName(ProfileName);
}

void AShell::Active()
{
	SetInitialVelocity();
	// SetActorHiddenInGame(false);
	BulletNS->SetVisibility(true);
	bActive = true;
}

void AShell::SetInitialVelocity()
{
	CurrentVelocity = GetActorForwardVector() * ShellInfo.ShellSpeed;

	if (ProjectileMovement)
	{
		ProjectileMovement->bSweepCollision = true;
		ProjectileMovement->bRotationFollowsVelocity = true;
		ProjectileMovement->bShouldBounce = false;
		ProjectileMovement->bIsHomingProjectile = false;
		ProjectileMovement->ProjectileGravityScale = 1;
		ProjectileMovement->Velocity = CurrentVelocity;
		ProjectileMovement->InitialSpeed = ShellInfo.ShellSpeed;
        ProjectileMovement->SetUpdatedComponent(Collision);
        ProjectileMovement->Activate(true);
	}
}

void AShell::SetTarget(class UClass* TargetClass)
{
	ShellTargetClass = TargetClass;
}
