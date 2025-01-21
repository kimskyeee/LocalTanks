#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OccupationComponent.generated.h"

UCLASS()
class UOccupationComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UOccupationComponent();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Occupation")
	void StartOccupationDelayed();
	UFUNCTION(BlueprintCallable, Category = "Occupation")
	void StopOccupationDelayed();
	UFUNCTION(BlueprintCallable, Category = "Occupation")
	bool IsOccupyingAvailable();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Occupation")
	FTimerHandle OccupationTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Occupation")
	float OccupationTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Occupation")
	bool bOccupyingAvailable = true;
};
