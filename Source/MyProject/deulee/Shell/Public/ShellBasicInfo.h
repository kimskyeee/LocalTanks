#pragma once

#include "CoreMinimal.h"
#include "EShellID.h"
#include "ShellBasicInfo.generated.h"

USTRUCT(BlueprintType)
struct FShellBasicInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	EShellID ShellID = EShellID::None;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	float ShellSpeed = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	float ShellDamage = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	float Penetration = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	float ShellRadius = 0.0f;
};
