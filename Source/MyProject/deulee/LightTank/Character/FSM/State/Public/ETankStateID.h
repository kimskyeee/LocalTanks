#pragma once

#include "CoreMinimal.h"
#include "ETankStateID.generated.h"

UENUM(BlueprintType)
enum class ETankStateID : uint8
{
	None UMETA(DisplayName = "None"), // None
	Idle UMETA(DisplayName = "Idle"), // In Center
	MoveToCenter UMETA(DisplayName = "Move to Center"), // Move to Center
	ApproachEnemy UMETA(DisplayName = "Approach Enemy"), // Approach Enemy
	AttackEnemy UMETA(DisplayName = "Attack Enemy"), // Attack Enemy
	Retreating UMETA(DisplayName = "Retreating"), // Retreating
	Damaged UMETA(DisplayName = "Damaged"), // Damaged
	Destroyed UMETA(DisplayName = "Destroyed"), // Destroyed
	TopAlignment UMETA(DisplayName = "Top Alignment"), // Top Alignment
};
