#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShellDecalComponent.generated.h"

UCLASS()
class UShellDecalComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UShellDecalComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "ShellDecal")
	void SpawnDecal(const FHitResult& HitResult, bool bHitTank);

private:
	UPROPERTY()
	class UMaterialInstanceDynamic* ShellDecalMaterial;

	UPROPERTY()
	class UMaterialInstance* ShellDecalMaterialInstance;
};
