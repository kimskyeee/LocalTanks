#pragma once

#include "CoreMinimal.h"
#include "AITank.h"
#include "DamageInterface.h"
#include "TeamInterface.h"
#include "MyPawn.generated.h"

UCLASS()
class AMyPawn : public AAITank, public IDamageInterface, public ITeamInterface
{
	GENERATED_BODY()
	
public:
	AMyPawn();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float GetCurrentHealth_Implementation() override;
	virtual float GetMaxHealth_Implementation() override;
	virtual bool TakeDamage_Implementation(FMyDamageStructure DamageInfo) override;
	virtual bool IsDead_Implementation() override;
	virtual bool IsPossibleToOccpupy_Implementation() override;
	virtual ETeam GetTeam_Implementation() const override { return ETeam::AI; }

	UFUNCTION(BlueprintCallable, Category = "MyPawn")
	void Fire(FVector ShellLocation, FRotator ShellRotation);
protected:
	// 게임 시작시 호출
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyeePawn")
	class UOccupationComponent* OccupationComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyeePawn")
	class UArmor* Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyeePawn")
	EShellID ShellID = EShellID::Pzgr41;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	FName ShellProfileName = TEXT("Enemy_Missile");
};
