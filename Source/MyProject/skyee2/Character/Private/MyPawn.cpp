#include "MyPawn.h"

#include "MyPawn.h"

#include "FastLogger.h"
#include "OccupationComponent.h"
#include "UArmor.h"
#include "MyProject/Mk/Character/Public/Mk_TankPawn.h"

AMyPawn::AMyPawn()
{
	// 틱 함수 활성화
	PrimaryActorTick.bCanEverTick = true;

	OccupationComponent = CreateDefaultSubobject<UOccupationComponent>(TEXT("OccupationComponent"));
	Armor = CreateDefaultSubobject<UArmor>(TEXT("Armor"));
}

void AMyPawn::Fire(FVector ShellLocation, FRotator ShellRotation)
{
	Armor->FireShell(ShellID, ShellLocation, ShellRotation, ShellProfileName, AMk_TankPawn::StaticClass());
	AttackDelegate.OnAttackDelegate.Broadcast();
}

void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AMyPawn::GetCurrentHealth_Implementation()
{
	return IDamageInterface::GetCurrentHealth_Implementation();
}

float AMyPawn::GetMaxHealth_Implementation()
{
	return IDamageInterface::GetMaxHealth_Implementation();
}

bool AMyPawn::TakeDamage_Implementation(FMyDamageStructure DamageInfo)
{
	OccupationComponent->StartOccupationDelayed();
	return IDamageInterface::TakeDamage_Implementation(DamageInfo);
}

bool AMyPawn::IsDead_Implementation()
{
	return IDamageInterface::IsDead_Implementation();
}

bool AMyPawn::IsPossibleToOccpupy_Implementation()
{
	return OccupationComponent->IsOccupyingAvailable();
}
