#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CoverComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UCoverComponent : public USceneComponent
{
public:
	GENERATED_BODY()

	UCoverComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Cover")
	void SetCoverValue();

protected:
	// 차폐의 정도 : 0.0f ~ 1.0f 사이의 값 : 0.0f는 완전히 차폐되지 않은 상태, 1.0f는 완전히 차폐된 상태
	UPROPERTY(EditAnywhere, Category = "Cover")
	float CoverValue = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Cover")
	bool CoverRender = true;

	UPROPERTY(EditAnywhere, Category = "Cover")
	FTimerHandle CoverTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Cover")
	class AActor* OuterCache = nullptr;

	UPROPERTY(EditAnywhere, Category = "Cover")
	TArray<class UStaticMeshComponent*> MeshComponents;

	UPROPERTY(EditAnywhere, Category = "Cover")
	TArray<class USkeletalMeshComponent*> SkeletalMeshComponents;

	UPROPERTY(EditAnywhere, Category = "Cover")
	TArray<class UMaterialInstanceDynamic*> OutlineMaterialInstances;
};
