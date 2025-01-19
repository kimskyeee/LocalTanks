#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TankState.h"
#include "UMoveToCenterState.generated.h"

UCLASS()
class UMoveToCenterState : public UTankState
{
	GENERATED_BODY()
public:
	UMoveToCenterState();

	virtual void Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual void Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime) override;

	virtual void Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual ETankStateID GetID() const override
	{return ETankStateID::MoveToCenter;};

protected:
	bool IsPossibleToApproach(ALightTankCharacter* Tank);
	void SetCenter();
	
	// UPROPERTY()
	// class UAStarPathFinding* PathFinding;
	UPROPERTY()
	float ApproachDistance = 4500.f;

	UPROPERTY()
	FVector Center = FVector(0, 0, 0);
};
