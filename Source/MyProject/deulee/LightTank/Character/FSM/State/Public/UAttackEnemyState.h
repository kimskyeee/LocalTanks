#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UTankTopAlignmentState.h"
#include "UAttackEnemyState.generated.h"

UCLASS()
class UAttackEnemyState : public UTankTopAlignmentState
{
	GENERATED_BODY()
public:
	UAttackEnemyState()
	{
		StateString = "AttackEnemy";
	}
	
	virtual void Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual void Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime) override;

	virtual void Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual ETankStateID GetID() const override
	{return ETankStateID::AttackEnemy;};

protected:
	bool IsPossibleToApproach(ALightTankCharacter* Tank);

	// @fixme : Get from Tank
	UPROPERTY()
	float ApproachDistance = 6500.f;
};
