#pragma once

#include "CoreMinimal.h"
#include "ETankRoleID.h"
#include "GameFramework/Actor.h"
#include "RespawnManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIDefeated, ETeam, WinningTeam);

USTRUCT(BlueprintType)
struct FTimerArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TArray<FTimerHandle> TimerHandles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TArray<bool> TimerActiveCache;
};

// 스테이지 1의 경우 (사실 이것만 있음)
UCLASS()
class ARespawnManager : public AActor
{
	GENERATED_BODY()
public:
	ARespawnManager();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void RespawnTank(ETankRoleID TankRoleID);

	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCountEnemyTank(ETankRoleID TankRoleID, int32 Count);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	FOnAIDefeated OnAIDefeatedDelegate;

protected:
	void CheckGameEnded();
	int32 FindTimerIndex(ETankRoleID TankRoleID);
	void RespawnPlayer(ETankRoleID TankRoleID);
	void RespawnHider(ETankRoleID TankRoleID);
	void RespawnRusher(ETankRoleID TankRoleID);
	void RespawnSniper(ETankRoleID TankRoleID);
	void UpdateRemainAllEnemies();

	void SpawnPlayer();
	void SpawnHider();
	void SpawnRusher();
	void SpawnSniper();

	void SpawnTankBeginPlay();
	APawn* SpawnActorAtRandomPlace(UClass* SpawnClass, FTransform& T);

	TMap<ETankRoleID, void (ARespawnManager::*)(ETankRoleID)> RespawnStrategies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TMap<ETankRoleID, float> RespawnTimers;

	// 개체마다 하나씩 가지고 있자.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TMap<ETankRoleID, FTimerArray> RespawnTimerHandles;

	// 스나이퍼의 경우 태어날 수 있는 스폰 위치가 정해져 있음
	// 나머지는 없음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TArray<FVector> SniperSpawnLocations;

	// SkyTankClass : Hider, Rusher
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TSubclassOf<class AMyPawn> SkyTankClass;

	// DeuleeTankClass : Sniper, Rusher
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TSubclassOf<class ALightTankCharacter> AI_LightTankClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TSubclassOf<class AMk_TankPawn> PlayerTankClass;

	// 저격수의 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 SniperCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 SniperMaxInMap = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 SniperMax = 2;

	// 숨는 탱크의 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 HiderCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 HiderMaxInMap = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 HiderMax = 4;

	// 돌진 탱크의 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 RusherCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 RusherMaxInMap = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 RusherMax = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 SkyTankClassCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	int32 LightTankClassCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	TSubclassOf<class UUserWidget> RespawnWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	class UUserWidget* RespawnWidgetInstance;
};
