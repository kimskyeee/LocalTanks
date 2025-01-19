#pragma once

#include "CoreMinimal.h"
#include "ETeam.h"
#include "UObject/Interface.h"
#include "TeamInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UTeamInterface : public UInterface
{
	GENERATED_BODY()
};

class ITeamInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Blueprintable, BlueprintNativeEvent, Category = "TeamInterface")
	ETeam GetTeam() const;
};
