#pragma once

#include "CoreMinimal.h"
#include "AttackDelegate.h"
#include "GameFramework/Actor.h"
#include "AttackDelegate.h"
#include "CoverActorBase.generated.h"

// 이 클래스는 위장 가능한지 여부를 가지고 있고 위장 가능 상태에서는 수풀이 보이며
// 위장 불가능한 상태에서는 수풀이 보이지 않는다.
UCLASS()
class ACoverActorBase : public AActor
{
	GENERATED_BODY()
public:
	bool IsCoverable();

	UFUNCTION()
	virtual void BindAttackDelegate(FAttackDelegate& AttackDelegate) {}; 

protected:
	bool bCoverable = true;
};
