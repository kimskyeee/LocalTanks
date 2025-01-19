#pragma once

#include "CoreMinimal.h"
#include "AShell.h"
#include "MyDamageStructure.h"
#include "ShellDamageEvent.h"
#include "APShell.generated.h"

UCLASS()
class APShell : public AShell
{
	GENERATED_BODY()
public:
	APShell();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Shell")
	virtual void OnShellOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// FShellDamageEvent ShellDamageEvent;
private:
	void WrapShellDamageInfo();

	UPROPERTY()
	FMyDamageStructure DamageInfo;
};
