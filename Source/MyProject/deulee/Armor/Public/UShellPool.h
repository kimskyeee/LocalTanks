#pragma once

#include "CoreMinimal.h"
#include "EShellID.h"
#include "UObject/Object.h"
#include "UShellPool.generated.h"

class AShell;
class UArmor;

USTRUCT()
struct FShellArray
{
	GENERATED_BODY()
	
	TArray<AShell*> Shells;
};

UCLASS()
class UShellPool : public UObject
{
	GENERATED_BODY()
public:
	UShellPool();

	UFUNCTION(BlueprintCallable, Category = "Armor")
	AShell* AcquireShell(EShellID ShellID, UArmor* ArmorRef);
	UFUNCTION(BlueprintCallable, Category = "Armor")
	void ReleaseShell(EShellID ShellID, AShell* Shell);
	
protected:
	UPROPERTY(Blueprintable, EditAnywhere, Category = "Armor")
	TMap<EShellID, FShellArray> Pool;
};
