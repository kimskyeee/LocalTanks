#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AttackDelegate.h"
#include "ETankRoleID.h"
#include "AITank.generated.h"

UCLASS()
class AAITank : public APawn
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	void SetRoleID(ETankRoleID InRoleID) {this->RoleID = InRoleID;}
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	ETankRoleID GetRoleID() const {return RoleID;}

protected:
	// 역할 : 저격수, 돌격
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	ETankRoleID RoleID;
};
