// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/Mk/Character/Public/Mk_TankPawn.h"

#include "OccupationComponent.h"


// Sets default values
AMk_TankPawn::AMk_TankPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OccupationComponent = CreateDefaultSubobject<UOccupationComponent>(TEXT("OccupationComponent"));
}

// Called when the game starts or when spawned
void AMk_TankPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMk_TankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMk_TankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AMk_TankPawn::GetCurrentHealth_Implementation()
{
	return IDamageInterface::GetCurrentHealth_Implementation();
}

float AMk_TankPawn::GetMaxHealth_Implementation()
{
	return IDamageInterface::GetMaxHealth_Implementation();
}

bool AMk_TankPawn::TakeDamage_Implementation(FMyDamageStructure DamageInfo)
{
	OccupationComponent->StartOccupationDelayed();
	return IDamageInterface::TakeDamage_Implementation(DamageInfo);
}

bool AMk_TankPawn::IsDead_Implementation()
{
	return IDamageInterface::IsDead_Implementation();
}

bool AMk_TankPawn::IsPossibleToOccpupy_Implementation()
{
	return OccupationComponent->IsOccupyingAvailable();
}

