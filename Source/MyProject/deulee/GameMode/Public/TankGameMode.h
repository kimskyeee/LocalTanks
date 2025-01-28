#pragma once

#include "CoreMinimal.h"
#include "ETeam.h"
#include "GameFramework/GameModeBase.h"
#include "TankGameMode.generated.h"

UCLASS()
class ATankGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ATankGameMode();

	UFUNCTION(BlueprintCallable, Category = "TankGameMode")
	void RespawnTank(AActor* DestroyedActor);

protected:
	UFUNCTION()
	APawn* SpawnActorAtRandomPlace(class UClass* Class);
	
	UFUNCTION()
	void OnWinnerDetected(ETeam WinningTeam);
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TankGameMode")
	TSubclassOf<class UOutcomeUI> PlayerWinnerWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TankGameMode")
	TSubclassOf<class UOutcomeUI> AIWinnerWidgetClass;

	UPROPERTY()
	UOutcomeUI* PlayerWinnerWidget = nullptr;
	UPROPERTY()
	UOutcomeUI* AIWinnerWidget = nullptr;

	UPROPERTY()
	TArray<class ALightTankCharacter*> AI_LightTanks;
	
	UPROPERTY()
	TArray<class AMyPawn*> AI_SkyTanks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TankGameMode")
	TSubclassOf<class AMyPawn> SkyTankClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TankGameMode")
	TSubclassOf<class ALightTankCharacter> AI_LightTankClass;
	
	UPROPERTY()
	class AMk_TankPawn* PlayerTank;
	
	// Spawn 장소들의 배열 (이 중 하나가 랜덤으로 선택되어 탱크가 생성됨)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TankGameMode")
	TArray<FVector> SpawnLocations;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TankGameMode")
	TArray<class ACoverActorBase*> CoverActors;

	int32 PrevSpawnIndex = -1;
};
