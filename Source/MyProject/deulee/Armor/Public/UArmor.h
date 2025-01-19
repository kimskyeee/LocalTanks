#pragma once

#include "CoreMinimal.h"
#include "EShellID.h"
#include "Components/ActorComponent.h"
#include "UArmor.generated.h"

class AShell;

UCLASS()
class UArmor : public UActorComponent
{
	GENERATED_BODY()
public:
	UArmor();
	
	UFUNCTION(BlueprintCallable, Category = "Armor")
	void FireShell(EShellID ShellID, FVector Location, FRotator Rotation, FName ProfileName);
	UFUNCTION(BlueprintCallable, Category = "Armor")
	void ReleaseShell(EShellID ShellID, AShell* Shell);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Armor")
	class UShellPool* ShellPool;
};
