#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoverActorBase.generated.h"

UCLASS()
class ACoverActorBase : public AActor
{
	GENERATED_BODY()
public:
	bool IsCoverable();

protected:
	bool bCoverable = true;
};
