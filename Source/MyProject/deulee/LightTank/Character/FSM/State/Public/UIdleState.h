#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TankState.h"
#include "UIdleState.generated.h"

UCLASS()
class UIdleState : public UTankState
{
	GENERATED_BODY()
public:
	UIdleState()
	{
		StateString = "Idle";
	}
	
	virtual void Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual void Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime) override;

	virtual void Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual ETankStateID GetID() const override
	{return ETankStateID::Idle;};
};
