#pragma once

#include "CoreMinimal.h"
#include "ShellBasicInfo.h"
#include "MyDamageStructure.generated.h"

USTRUCT(BlueprintType)
struct FMyDamageStructure
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FShellBasicInfo ShellBasicInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DamageLocation = FVector::ZeroVector;

	// bool bAbove
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAbove = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ImpactNormal = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpactDistance = 0.0f;
	
};
