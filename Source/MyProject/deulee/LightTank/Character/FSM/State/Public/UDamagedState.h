#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TankState.h"
#include "UDamagedState.generated.h"

UCLASS()
class UDamagedState : public UTankState
{
	GENERATED_BODY()
public:
	UDamagedState()
	{
		StateString = "Damaged";
	}
	
	virtual void Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual void Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime) override;

	virtual void Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual ETankStateID GetID() const override
	{return ETankStateID::Damaged;};
};
