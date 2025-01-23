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

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Armor")
	void FireShell(EShellID ShellID, FVector Location, FRotator Rotation, FName ProfileName, UClass* TargetClass);
	UFUNCTION(BlueprintCallable, Category = "Armor")
	void ReleaseShell(EShellID ShellID, AShell* Shell);

protected:
	UFUNCTION(BlueprintCallable, Category = "Armor")
	void ProjectileShot(EShellID ShellID, FVector Location, FRotator Rotation, FName ProfileName, UClass* TargetClass);
	UFUNCTION(BlueprintCallable, Category = "Armor")
	void HitscanShot(EShellID ShellID, const FHitResult& HitResult, UClass* TargetClass);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Armor")
	class UShellPool* ShellPool;
};
