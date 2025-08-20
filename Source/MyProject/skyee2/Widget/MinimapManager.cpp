// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapManager.h"
#include "MinimapCaptureActor.h"
#include "MinimapWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
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
	// 캡처 컴포넌트 유효성 확인
	if (!CapActor.Get()) return false;
	const USceneCaptureComponent2D* Capture = CapActor->GetCaptureComponent2D();
	if (!Capture) return false;

	// 오소그래픽 모드/폭 확인
	if (Capture->ProjectionType != ECameraProjectionMode::Orthographic) return false;
	const float OrthoWidth = Capture->OrthoWidth;
	if (OrthoWidth <= KINDA_SMALL_NUMBER) return false;

	// 렌더 타겟 종횡비로 오소그래픽 "세로 높이" 산출
	const UTextureRenderTarget2D* RT = Capture->TextureTarget;
	const int32 SizeX = (RT ? RT->SizeX : 1024);
	const int32 SizeY = (RT ? RT->SizeY : 1024);
	if (SizeX <= 0 || SizeY <= 0) return false;

	const float Aspect = static_cast<float>(SizeX) / static_cast<float>(SizeY); // 가로/세로
	const float OrthoHeight = OrthoWidth / FMath::Max(Aspect, KINDA_SMALL_NUMBER);

	const float HalfW = OrthoWidth * 0.5f;
	const float HalfH = OrthoHeight * 0.5f;

	// 월드 좌표 → 카메라 기준 평면(XY) 상의 오프셋
	const FVector CamLoc = Capture->GetComponentLocation();
	const FVector2D WorldPos(WorldLoc.X, WorldLoc.Y);
	const FVector2D CamPos(CamLoc.X,  CamLoc.Y);
	const FVector2D Delta = WorldPos - CamPos; // 카메라 중심을 원점으로 본 월드 오프셋

	// 카메라의 Right/Up 벡터(평면 투영)로 성분 투영 → 카메라 화면 좌/우, 상/하로 분해
	const FVector2D Right2D(Capture->GetRightVector().X, Capture->GetRightVector().Y); // 화면 가로축
	const FVector2D Up2D(Capture->GetUpVector().X, Capture->GetUpVector().Y); // 화면 세로축

	const float Xr = FVector2D::DotProduct(Delta, Right2D); // [-HalfW, +HalfW]
	const float Yu = FVector2D::DotProduct(Delta, Up2D);  // [-HalfH, +HalfH]

	// 정규화 [0,1]
	float U = (Xr + HalfW) / (OrthoWidth); // 가로 방향
	float V = (Yu + HalfH) / (OrthoHeight); // 세로 방향

	// 화면 밖이면 클램프
	// U = FMath::Clamp(U, 0.f, 1.f);
	// V = FMath::Clamp(V, 0.f, 1.f);
	// 화면 밖이면, UI에 표시하지 않기 위해 클램프를 생략한다
	
	OutUV = FVector2D(U, V);
	return true;
}

