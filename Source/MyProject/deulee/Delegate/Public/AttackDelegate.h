#pragma once

#include "CoreMinimal.h"
#include "AttackDelegate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackDelegate);

USTRUCT(BlueprintType)
struct FAttackDelegate
{
	GENERATED_BODY()

	FOnAttackDelegate OnAttackDelegate;
};