#pragma once

#include "CoreMinimal.h"
#include "EShellID.h"
#include "ShellBasicInfo.h"
#include "ShellLibrary.generated.h"

UCLASS(BlueprintType)
class UShellLibrary : public UObject
{
public:
	GENERATED_BODY()

	UShellLibrary();

	UFUNCTION(BlueprintCallable, Category = "Shell")
	static FShellBasicInfo GetShellBasicInfo(EShellID ShellID);
	UFUNCTION(BlueprintCallable, Category = "Shell")
	static UClass* GetShellClass(EShellID ShellID);

protected:
	static TMap<EShellID, FShellBasicInfo> ShellBasicInfoMap;
	static TMap<EShellID, UClass*> ShellClassMap;
};
