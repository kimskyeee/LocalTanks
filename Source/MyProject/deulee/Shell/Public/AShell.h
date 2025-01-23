#pragma once

#include "CoreMinimal.h"
#include "ShellBasicInfo.h"
#include "GameFramework/Actor.h"
#include "AShell.generated.h"

UCLASS()
class AShell : public AActor
{
	GENERATED_BODY()
public:
	AShell();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Shell")
	void SetShellInfo(FShellBasicInfo ShellInfo);
	UFUNCTION(BlueprintCallable, Category = "Shell")
	FShellBasicInfo GetShellInfo();
	UFUNCTION(BlueprintCallable, Category = "Shell")
	void SetArmor(class UArmor* Armor);
	UFUNCTION(BlueprintCallable, Category = "Shell")
	void SetInitialVelocity();
	UFUNCTION(BlueprintCallable, Category = "Shell")
	void SetTarget(class UClass* TargetClass);

	UFUNCTION(BlueprintCallable, Category = "Shell")
	void Active();
	UFUNCTION(BlueprintCallable, Category = "Shell")
	void DeActive();

	UFUNCTION(BlueprintCallable, Category = "Shell")
	void SetCollisionPreset(FName ProfileName);
	
	UFUNCTION(Blueprintable, Category = "Shell")
	virtual void HitScanShot(const FHitResult& HitResult){};
	
	virtual ~AShell() override {}

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	class UArmor* Armor;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	class UBoxComponent* Collision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	FShellBasicInfo ShellInfo;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	FVector CurrentVelocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	class UNiagaraComponent* BulletNS;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	class UShellDecalComponent* ShellDecalComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	TArray<UNiagaraComponent*> ExplosionNSPool;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	TArray<FTimerHandle> ExplosionTimerHandles;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	class UNiagaraSystem* ExplosionNSAsset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	TSubclassOf<class AActor> ShellTargetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	int32 ExplosionPoolSize = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	FTimerHandle DeActiveTimerHandle;

	UPROPERTY()
	bool bCollisionActive = false;
};
