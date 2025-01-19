#pragma once

#include "CoreMinimal.h"
#include "EShellID.h"
#include "ShellBasicInfo.generated.h"

USTRUCT(BlueprintType)
struct FShellBasicInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	EShellID ShellID;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	float ShellSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	float ShellDamage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	float Penetration;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	float ShellRadius;
};
