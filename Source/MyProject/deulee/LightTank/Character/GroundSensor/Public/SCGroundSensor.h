#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SCGroundSensor.generated.h"

UCLASS()
class USCGroundSensor : public USceneComponent
{
	GENERATED_BODY() 
public:
	USCGroundSensor();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Ground Sensor")
	FVector GetHitLocation() const { return HitLocation; }
	UFUNCTION(BlueprintCallable, Category = "Ground Sensor")
	FVector GetHitNormal() const { return HitNormal; }
	UFUNCTION(BlueprintCallable, Category = "Ground Sensor")
	bool IsOnGround() const { return bOnGround; }
	
protected:
	float TraceDistance = 20000.0f;
	bool bOnGround;
	FVector HitLocation;
	FVector HitNormal;
};
