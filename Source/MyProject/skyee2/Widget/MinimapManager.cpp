// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapManager.h"
#include "MinimapCaptureActor.h"
#include "MinimapWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AMinimapManager::AMinimapManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMinimapManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (!CapActor.IsValid())
	{
		// 없다면, 월드에 배치된 액터를 찾는다
		TArray<AActor*> FoundActor;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMinimapCaptureActor::StaticClass(), FoundActor);
		if (FoundActor.Num() > 0)
		{
			CapActor = Cast<AMinimapCaptureActor>(FoundActor[0]);
		}
	}
	
	CapActor->MinimapCapture = RenderTarget; // RT 설정
	CapActor->TakeOneShotCapture(true);

	UE_LOG(LogTemp, Warning, TEXT("AMinimapManager Minimap Capture Complete"));

	CollectActorsByClass(); // 마커 갱신할 대상 수집
	
	// AI는 랜덤스폰 되므로, 타이머 돌리자
	// AI 스폰 로직에서 실행해주는 것도 좋을듯 
	GetWorld()->GetTimerManager().SetTimer(RecollectHandle, this, &AMinimapManager::CollectActorsByClass, 1.0f, true);

	if (MinimapWidgetClass)
	{
		MinimapWidget = CreateWidget<UMinimapWidget>(GetWorld(), MinimapWidgetClass);
		if (MinimapWidget)
		{
			MinimapWidget->MinimapImageTexture = RenderTarget;
			UE_LOG(LogTemp, Warning, TEXT("RenderTarget Set Complete"));
			MinimapWidget->Manager = this;
			MinimapWidget->AddToViewport();
		}
	}
}

// Called every frame
void AMinimapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMinimapManager::CollectActorsByClass()
{
	// 전체 재수집
	Players.Reset();
	AIs.Reset();
	Destinations.Reset();

	auto Collect = [&](TSubclassOf<AActor> CollectClass, TArray<TWeakObjectPtr<AActor>>& Out)
	{
		if (!*CollectClass) return;
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), *CollectClass, Found);

		// 용량 미리 확보 (재할당 줄이기)
		Out.Reserve(Out.Num() + Found.Num());
		
		for (AActor* A : Found) Out.Add(A);
	};

	Collect(PlayerClass, Players);
	Collect(AIClass, AIs);
	Collect(DestinationClass, Destinations);
}

bool AMinimapManager::WorldToMinimapUV(const FVector& WorldLoc, FVector2D& OutUV) const
{
	// 월드 좌표(WorldLoc)를 미니맵 좌표계(정규화된 UV, 0~1 범위)로 변환

	// 미니맵이 커버할 월드 XY 범위
	const FVector2D Min = MapBounds.Min;
	const FVector2D Max = MapBounds.Max;

	// 맵의 폭과 높이(DX, DY)를 구하기
	const float DX = (Max.X - Min.X);
	const float DY = (Max.Y - Min.Y);
	if (DX <= KINDA_SMALL_NUMBER || DY <= KINDA_SMALL_NUMBER) return false;

	// 정규화
	OutUV = FVector2D(
		(WorldLoc.X - Min.X) / DX,
		(WorldLoc.Y - Min.Y) / DY
	);
	
	return true;
}

