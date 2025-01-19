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
	FVector DamageLocation;

	// bool bAbove
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAbove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ImpactNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpactDistance;
	
};
