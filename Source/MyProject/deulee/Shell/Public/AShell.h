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

	UFUNCTION(BlueprintCallable, Category = "Shell")
	void SetShellInfo(FShellBasicInfo ShellInfo);
	UFUNCTION(BlueprintCallable, Category = "Shell")
	FShellBasicInfo GetShellInfo();
	UFUNCTION(BlueprintCallable, Category = "Shell")
	void SetArmor(class UArmor* Armor);
	UFUNCTION(BlueprintCallable, Category = "Shell")
	void SetInitialVelocity();

	UFUNCTION(BlueprintCallable, Category = "Shell")
	void Active();
	UFUNCTION(BlueprintCallable, Category = "Shell")
	void DeActive();

	UFUNCTION(BlueprintCallable, Category = "Shell")
	void SetCollisionPreset(FName ProfileName);
	
	virtual ~AShell() override {}
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	class UArmor* Armor;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	class UBoxComponent* Collision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	FShellBasicInfo ShellInfo;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	bool bActive;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	FVector CurrentVelocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	class UNiagaraComponent* BulletNS;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shell")
	class UShellDecalComponent* ShellDecalComponent;
};

