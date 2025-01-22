#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACLightTankFSM.generated.h"

class UTankState;

UCLASS()
class UACLightTankFSM : public UActorComponent
{
	GENERATED_BODY()
public:
	UACLightTankFSM();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Tank State")
	void ChangeState(ETankStateID NewStateID);
	
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	UTankState* GetCurrentState();
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	UTankState* GetPrevState();
	
protected:
	void InitStatePool();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank State")
	class ALightTankCharacter* OwnerActor;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank State")
	UTankState* CurrentState;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank State")
	UTankState* PrevState;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank State")
	TMap<ETankStateID, UTankState*> StatePool;
};
