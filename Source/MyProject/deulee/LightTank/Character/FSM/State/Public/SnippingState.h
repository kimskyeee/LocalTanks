#pragma once

#include "CoreMinimal.h"
#include "UTankTopAlignmentState.h"
#include "SnippingState.generated.h"

UCLASS()
class USnippingState : public UTankTopAlignmentState
{
	GENERATED_BODY()
public:
	USnippingState()
	{
		StateString = "Snipping";
	}

	virtual void Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;
	virtual void Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime) override;
	virtual void Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual ETankStateID GetID() const override
	{
		return ETankStateID::Snipping;
	};
};
