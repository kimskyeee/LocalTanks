#pragma once

#include "CoreMinimal.h"
#include "DamageInterface.h"
#include "TeamInterface.h"
#include "MyPawn.generated.h"

UCLASS()
class AMyPawn : public APawn, public IDamageInterface, public ITeamInterface
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
protected:
	// 게임 시작시 호출
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyeePawn")
	class UOccupationComponent* OccupationComponent;
};
