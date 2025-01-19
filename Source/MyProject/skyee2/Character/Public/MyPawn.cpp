#include "MyPawn.h"

#include "MyPawn.h"

AMyPawn::AMyPawn()
{
	// 틱 함수 활성화
	PrimaryActorTick.bCanEverTick = true;
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
	return IDamageInterface::TakeDamage_Implementation(DamageInfo);
}

bool AMyPawn::IsDead_Implementation()
{
	return IDamageInterface::IsDead_Implementation();
}

bool AMyPawn::IsPossibleToOccpupy_Implementation()
{
	return IDamageInterface::IsPossibleToOccpupy_Implementation();
}
