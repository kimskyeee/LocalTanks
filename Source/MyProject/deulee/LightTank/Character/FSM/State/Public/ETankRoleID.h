#pragma once

#include "CoreMinimal.h"
#include "ETankRoleID.generated.h"

UENUM(BlueprintType)
enum class ETankRoleID : uint8
{
	None UMETA(DisplayName = "None"),
	Rusher UMETA(DisplayName = "Rusher"),
	Sniper UMETA(DisplayName = "Sniper"),
	Hider UMETA(DisplayName = "Hider"),
};
