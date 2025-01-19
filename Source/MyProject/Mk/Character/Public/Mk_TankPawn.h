// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageInterface.h"
#include "TeamInterface.h"
#include "GameFramework/Pawn.h"
#include "WheeledVehiclePawn.h"
#include "Mk_TankPawn.generated.h"

UCLASS()
class MYPROJECT_API AMk_TankPawn : public AWheeledVehiclePawn, public IDamageInterface, public ITeamInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMk_TankPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float GetCurrentHealth_Implementation() override;
	virtual float GetMaxHealth_Implementation() override;
	virtual bool TakeDamage_Implementation(FMyDamageStructure DamageInfo) override;
	virtual bool IsDead_Implementation() override;
	virtual bool IsPossibleToOccpupy_Implementation() override;

	virtual ETeam GetTeam_Implementation() const override { return ETeam::Player; }
};
