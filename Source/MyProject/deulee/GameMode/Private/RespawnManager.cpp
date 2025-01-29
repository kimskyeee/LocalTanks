#include "RespawnManager.h"

#include "ALightTankCharacter.h"
#include "FastLogger.h"
#include "MyPawn.h"
#include "MyProject/Mk/Character/Public/Mk_TankPawn.h"

ARespawnManager::ARespawnManager()
{
	RespawnStrategies.Add(ETankRoleID::Player, &ARespawnManager::RespawnPlayer);
	RespawnStrategies.Add(ETankRoleID::Hider, &ARespawnManager::RespawnHider);
	RespawnStrategies.Add(ETankRoleID::Rusher, &ARespawnManager::RespawnRusher);
	RespawnStrategies.Add(ETankRoleID::Sniper, &ARespawnManager::RespawnSniper);

	RespawnTimers.Add(ETankRoleID::Player, 3.f);
	RespawnTimers.Add(ETankRoleID::Hider, 7.f);
	RespawnTimers.Add(ETankRoleID::Rusher, 5.f);
	RespawnTimers.Add(ETankRoleID::Sniper, 10.f);

	RespawnTimerHandles.Add(ETankRoleID::Player, FTimerArray());
	RespawnTimerHandles.Add(ETankRoleID::Hider, FTimerArray());
	RespawnTimerHandles.Add(ETankRoleID::Rusher, FTimerArray());
	RespawnTimerHandles.Add(ETankRoleID::Sniper, FTimerArray());

	RespawnTimerHandles[ETankRoleID::Player].TimerHandles.SetNum(1);
	RespawnTimerHandles[ETankRoleID::Player].TimerActiveCache.SetNum(1);
	RespawnTimerHandles[ETankRoleID::Hider].TimerHandles.SetNum(HiderMaxInMap);
	RespawnTimerHandles[ETankRoleID::Hider].TimerActiveCache.SetNum(HiderMaxInMap);
	RespawnTimerHandles[ETankRoleID::Rusher].TimerHandles.SetNum(RusherMaxInMap);
	RespawnTimerHandles[ETankRoleID::Rusher].TimerActiveCache.SetNum(RusherMaxInMap);
	RespawnTimerHandles[ETankRoleID::Sniper].TimerHandles.SetNum(SniperMaxInMap);
	RespawnTimerHandles[ETankRoleID::Sniper].TimerActiveCache.SetNum(SniperMaxInMap);

	// SniperSpawnLocations
	SniperSpawnLocations.Add(FVector(-32397.732062, -18470.721525, 5309.040213));
}

void ARespawnManager::BeginPlay()
{
	Super::BeginPlay();
}

void ARespawnManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (auto& TimerArray : RespawnTimerHandles)
	{
		for (auto& TimerHandle : TimerArray.Value.TimerHandles)
		{
			if (TimerHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			}
		}
	}
}

void ARespawnManager::CheckGameEnded()
{
	if (HiderCount <= 0 && RusherCount <= 0 && SniperCount <= 0)
	{
		OnAIDefeatedDelegate.Broadcast(ETeam::Player);
	}
}

void ARespawnManager::RespawnTank(ETankRoleID TankRoleID)
{
	if (RespawnStrategies.Contains(TankRoleID))
	{
		auto RespawnFunc = RespawnStrategies[TankRoleID];
		(this->*RespawnFunc)(TankRoleID);
		UpdateRemainAllEnemies();
		CheckGameEnded();
	}
}

void ARespawnManager::StartGame()
{
	SpawnTankBeginPlay();
	
	UpdateRemainAllEnemies();
}

int32 ARespawnManager::FindTimerIndex(ETankRoleID TankRoleID)
{
	if (!RespawnTimers.Contains(TankRoleID) || !RespawnTimerHandles.Contains(TankRoleID))
	{
		return -1;
	}

	TArray<FTimerHandle>& TimerHandles = RespawnTimerHandles[TankRoleID].TimerHandles;
	TArray<bool>& TimerActiveCache = RespawnTimerHandles[TankRoleID].TimerActiveCache;

	int32 Index = -1;
	for (Index = 0; Index < TimerActiveCache.Num(); Index++)
	{
		if (!TimerActiveCache[Index])
		{
			TimerActiveCache[Index] = true;
			break ;
		}
	}

	// No Timer Available
	if (Index >= TimerHandles.Num())
	{
		return -1;
	}

	return Index;
}

void ARespawnManager::RespawnPlayer(ETankRoleID TankRoleID)
{
	int32 TimerIndex = FindTimerIndex(TankRoleID);
	if (TimerIndex == -1)
	{
		return ;
	}

	TWeakObjectPtr<ARespawnManager> WeakThis = this;
	FTimerHandle& TimerHandle = RespawnTimerHandles[TankRoleID].TimerHandles[TimerIndex];
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([WeakThis, TankRoleID, TimerIndex]()
	{
		if (WeakThis.IsValid())
		{
			ARespawnManager* StrongThis = WeakThis.Get();
			StrongThis->SpawnPlayer();
			StrongThis->RespawnTimerHandles[TankRoleID].TimerActiveCache[TimerIndex] = false;
		}
	}), RespawnTimers[TankRoleID], false);
}

void ARespawnManager::RespawnHider(ETankRoleID TankRoleID)
{
	if (HiderMax > 0)
	{
		HiderMax--;
	}
	else
	{
		HiderCount--;
		return ;
	}

	// UI 업데이트 : HiderMax + HiderCount : 현재 남은 숨는 탱크 수
	int32 TimerIndex = FindTimerIndex(TankRoleID);
	if (TimerIndex == -1)
	{
		return;
	}

	TWeakObjectPtr<ARespawnManager> WeakThis = this;
	FTimerHandle& TimerHandle = RespawnTimerHandles[TankRoleID].TimerHandles[TimerIndex];
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([WeakThis, TankRoleID, TimerIndex]()
	{
		if (WeakThis.IsValid())
		{
			ARespawnManager* StrongThis = WeakThis.Get();
			StrongThis->SpawnHider();
			StrongThis->RespawnTimerHandles[TankRoleID].TimerActiveCache[TimerIndex] = false;
		}
	}), RespawnTimers[TankRoleID], false);
}

void ARespawnManager::RespawnRusher(ETankRoleID TankRoleID)
{
	if (RusherMax > 0)
	{
		RusherMax--;
	}
	else
	{
		RusherCount--;
		return ;
	}

	// UI 업데이트 : RusherMax + RusherCount : 현재 남은 돌진 탱크 수
	int32 TimerIndex = FindTimerIndex(TankRoleID);
	if (TimerIndex == -1)
	{
		return;
	}

	TWeakObjectPtr<ARespawnManager> WeakThis = this;
	FTimerHandle& TimerHandle = RespawnTimerHandles[TankRoleID].TimerHandles[TimerIndex];
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([WeakThis, TankRoleID, TimerIndex]()
	{
		if (WeakThis.IsValid())
		{
			ARespawnManager* StrongThis = WeakThis.Get();
			StrongThis->SpawnRusher();
			StrongThis->RespawnTimerHandles[TankRoleID].TimerActiveCache[TimerIndex] = false;
		}
	}), RespawnTimers[TankRoleID], false);
}

void ARespawnManager::RespawnSniper(ETankRoleID TankRoleID)
{
	if (SniperMax > 0)
	{
		SniperMax--;
	}
	else
	{
		SniperCount--;
		return ;
	}

	// UI 업데이트 : SniperMax + SniperCount : 현재 남은 스나이퍼 수

	int32 TimerIndex = FindTimerIndex(TankRoleID);
	if (TimerIndex == -1)
	{
		return;
	}

	TWeakObjectPtr<ARespawnManager> WeakThis = this;
	FTimerHandle& TimerHandle = RespawnTimerHandles[TankRoleID].TimerHandles[TimerIndex];
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([WeakThis, TankRoleID, TimerIndex]()
	{
		if (WeakThis.IsValid())
		{
			ARespawnManager* StrongThis = WeakThis.Get();
			StrongThis->SpawnSniper();
			StrongThis->RespawnTimerHandles[TankRoleID].TimerActiveCache[TimerIndex] = false;
		}
	}), RespawnTimers[TankRoleID], false);
}

void ARespawnManager::SpawnPlayer()
{
	FTransform T;
	
	AMk_TankPawn* PlayerPawn = Cast<AMk_TankPawn>(SpawnActorAtRandomPlace(SkyTankClass, T));
	if (!PlayerPawn)
	{
		return ;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) // Null Guard
	{
		return ;
	}

	PlayerController->Possess(PlayerPawn);
}

void ARespawnManager::SpawnHider()
{
	FTransform T;
	AAITank* SpawnedPawn = Cast<AAITank>(SpawnActorAtRandomPlace(SkyTankClass, T));
	if (SpawnedPawn)
	{
		SpawnedPawn->SetRoleID(ETankRoleID::Hider);
		SpawnedPawn->FinishSpawning(T, true);
	}
}

void ARespawnManager::SpawnRusher()
{
	// UCLASS SkyTankClass, DeuleeTankClass를 반반 나눠서 스폰
	// 투 포인터
	UClass* SpawnClass = nullptr;
	if (SkyTankClassCount <= LightTankClassCount)
	{
		SpawnClass = SkyTankClass;
		SkyTankClassCount++;
	}
	else
	{
		SpawnClass = AI_LightTankClass;
		LightTankClassCount++;
	}

	FTransform T;
	AAITank* SpawnedPawn = Cast<AAITank>(SpawnActorAtRandomPlace(SpawnClass, T));
	if (SpawnedPawn)
	{
		SpawnedPawn->SetRoleID(ETankRoleID::Rusher);
		SpawnedPawn->FinishSpawning(T, true);
	}
}

void ARespawnManager::SpawnSniper()
{
	// 임시 하나밖에 없음
	FVector SpawnLocation = SniperSpawnLocations[0];

	FTransform T{};
	T.SetLocation(SpawnLocation);

	AAITank* SpawnedPawn = GetWorld()->SpawnActorDeferred<AAITank>(AI_LightTankClass, T);
	if (SpawnedPawn)
	{
		SpawnedPawn->SetRoleID(ETankRoleID::Sniper);
		SpawnedPawn->FinishSpawning(T, true);
	}
}

void ARespawnManager::SpawnTankBeginPlay()
{
	for (int32 i = 0; i < SniperMaxInMap; i++)
	{
		SpawnSniper();
		SniperMax--;
		SniperCount++;
	}

	for (int32 i = 0; i < HiderMaxInMap; i++)
	{
		SpawnHider();
		HiderMax--;
		HiderCount++;
	}

	for (int32 i = 0; i < RusherMaxInMap; i++)
	{
		SpawnRusher();
		RusherMax--;
		RusherCount++;
	}
}

APawn* ARespawnManager::SpawnActorAtRandomPlace(UClass* SpawnClass, FTransform& T)
{
	float Radius = FMath::RandRange(15000.f, 18000.f);
	float Angle  = FMath::RandRange(0.f, 2.f * PI);

	float RandomX = Radius * FMath::Cos(Angle);
	float RandomY = Radius * FMath::Sin(Angle);

	// Z값 초기화
	float SpawnZ = 0.f;

	// 2. Line Trace로 땅의 높이 확인
	FVector StartPoint = FVector(RandomX, RandomY, 10000.f); // 위에서 시작
	FVector EndPoint = FVector(RandomX, RandomY, -10000.f); // 아래로 쏘기

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 자신의 충돌 무시
	Params.bTraceComplex = true;
	
	// 라인 트레이스 실행
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility, Params))
	{
		// 땅의 Z 좌표를 가져옴
		SpawnZ = HitResult.Location.Z;
	}

	float Offset = 200.f;
	if (SpawnClass == AI_LightTankClass || SpawnClass == SkyTankClass)
	{
		Offset = 700.f;
	}
	// 3. Offset 추가
	SpawnZ += Offset;

	// 4. 최종 위치 계산
	FVector SpawnLocation = FVector(RandomX, RandomY, SpawnZ);

	// 5. 액터 스폰
	T.SetLocation(SpawnLocation);

	// 반드시 FinishSpawningActor를 호출해야 함
	APawn* SpawnPawn = GetWorld()->SpawnActorDeferred<APawn>(SpawnClass, T);
	if (SpawnPawn)
	{
		SpawnPawn->AutoPossessPlayer = EAutoReceiveInput::Disabled;
	}
	return SpawnPawn;
}

void ARespawnManager::UpdateRemainAllEnemies()
{
	UpdateCountEnemyTank(ETankRoleID::Hider, HiderCount + HiderMax);
	UpdateCountEnemyTank(ETankRoleID::Rusher, RusherCount + RusherMax);
	UpdateCountEnemyTank(ETankRoleID::Sniper, SniperCount + SniperMax);
}
