#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ETeam.h"
#include "OccupiedZone.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTankBeginOverlap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTankEndOverlap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWinnerDetected, ETeam, WinningTeam);

UCLASS()
class AOccupiedZone : public AActor
{
	GENERATED_BODY()
public:
	AOccupiedZone();

    virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnTankBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnTankEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "OccupiedZone")
	FOnTankBeginOverlap OnTankBeginOverlapDelegate;

	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "OccupiedZone")
	FOnTankEndOverlap OnTankEndOverlapDelegate;

	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "OccupiedZone")
	FOnWinnerDetected OnWinnerDetectedDelegate;
protected:
	ETeam DetermineControllingTeam();
	bool ValidateTanksState();
	void SetControllingTeam();
	void BroadCaseWinningTeam() const;
	void UpdateCaptureProgress(float DeltaSeconds);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OccupiedZone")
	class USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OccupiedZone")
	class UStaticMeshComponent* CylinderMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OccupiedZone")
	float CaptureRate = 5;

	// Player, AI
	// Control할 팀이 누군지 검사를 해야 함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OccupiedZone")
	ETeam ControllingTeam;

	// 0 ~ 100
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OccupiedZone")
	float CaptureProgress = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OccupiedZone")
	TArray<class AActor*> OverlappedTanks;
};
