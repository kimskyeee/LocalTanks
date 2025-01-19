#pragma once

#include "CoreMinimal.h"
#include "MyDamageStructure.h"
#include "UObject/Interface.h"
#include "DamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * The interface class that defines the functionality.
 */
class MYPROJECT_API IDamageInterface
{
	GENERATED_BODY()

public:
	// Example function to implement in the Blueprint
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DamageInterface")
	float GetCurrentHealth();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DamageInterface")
	float GetMaxHealth();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DamageInterface")
	bool TakeDamage(FMyDamageStructure DamageInfo);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DamageInterface")
	bool IsDead();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DamageInterface")
	bool IsPossibleToOccpupy();
};