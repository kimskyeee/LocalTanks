#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UTankTopAlignmentState.h"
#include "UApproachEnemyState.generated.h"

UCLASS()
class UApproachEnemyState : public UTankTopAlignmentState
{
	GENERATED_BODY()
public:
	UApproachEnemyState();
	
	virtual void Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual void Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime) override;

	virtual void Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual ETankStateID GetID() const override
	{return ETankStateID::ApproachEnemy;};

protected:
	// @fixme : Get from Tank
	UPROPERTY()
    float ApproachDistance = 6500.f;
};
