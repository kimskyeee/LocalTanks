#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoverActorBase.h"
#include "BushActor.generated.h"

UCLASS()
class ABushActor : public ACoverActorBase
{
	GENERATED_BODY()
public:
	ABushActor();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnAttackDetected();
	UFUNCTION()
	void ReCover();
	
	virtual void BindAttackDelegate(FAttackDelegate& AttackDelegate) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USphereComponent* SphereRoot = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* BushMesh = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UMaterialInstance* OriginalBark = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UMaterialInstance* OriginalLeaf = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UMaterialInstance* TransparencyBark = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UMaterialInstance* TransparencyLeaf = nullptr;

	UPROPERTY()
	FTimerHandle CoverHandle;
	
	UPROPERTY()
	bool bOverlap;
};
