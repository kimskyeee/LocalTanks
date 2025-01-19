#pragma once

#include "CoreMinimal.h"
#include "TankState.h"
#include "UTankTopAlignmentState.generated.h"

UCLASS()
class UTankTopAlignmentState : public UTankState
{
	GENERATED_BODY()
public:
	UTankTopAlignmentState();

	virtual void Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual void Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime) override;

	virtual void Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM) override;

	virtual ~UTankTopAlignmentState() override {};

	bool CanAttackTarget() const;
	
	virtual ETankStateID GetID() const override
	{return ETankStateID::TopAlignment;};
	
protected:
	void CheckAttackCondition(float TargetTheta, float CurrentTheta, bool Debug = false);
	void SetTurretTargetAngle(ALightTankCharacter* Tank);
	void SetGunTargetAngle(ALightTankCharacter* Tank);

	bool bCanAttackTarget = false;
};
