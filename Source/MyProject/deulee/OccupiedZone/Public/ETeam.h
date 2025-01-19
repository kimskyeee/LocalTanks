#pragma once

#include "CoreMinimal.h"
#include "ETeam.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	None = 0,
	Player = 1,
	AI = 2,
};
