#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UTankTopAlignmentState.h"
#include "URetreatingState.generated.h"

UCLASS()
class URetreatingState : public UTankTopAlignmentState
{
	GENERATED_BODY()
public:
	URetreatingState();
	
	virtual void Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual void Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime) override;

	virtual void Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual ETankStateID GetID() const override
	{return ETankStateID::Retreating;};

	UPROPERTY()
	bool bSkipSetting;
protected:
	// @fixme : Get from Tank
	UPROPERTY()
	float RetreatDistance = 7000.f;
};
