#pragma once

#include "CoreMinimal.h"
#include "ETankStateID.h"
#include "UObject/Object.h"
#include "TankState.generated.h"

class UACLightTankFSM;
class ALightTankCharacter;

UCLASS(Abstract, Blueprintable)
class UTankState : public UObject
{
	GENERATED_BODY()
public:
	UTankState();
	
	UFUNCTION(BlueprintCallable)
	virtual void Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM) {}

	UFUNCTION(BlueprintCallable)
	virtual void Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime) {}

	UFUNCTION(BlueprintCallable)
	virtual void Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM) {}

	UFUNCTION(BlueprintCallable)
	virtual ETankStateID GetID() const { return ETankStateID::None; }

	UFUNCTION(BlueprintCallable)
	FString GetStateName() const { return StateString; }

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank State")
	FString StateString = "None";
	
	UPROPERTY()
	class UAStarPathFinding* PathFinding;
};
