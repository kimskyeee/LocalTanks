// IndicatorWidget.cpp

#include "IndicatorWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "MyProject/skyee2/DestinationActor/DestinationActor.h"

// 내부 헬퍼
static FORCEINLINE bool NearlyZero2D(const FVector2D& V) { return FMath::IsNearlyZero(V.X) && FMath::IsNearlyZero(V.Y); }

void UIndicatorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	DestinationActor = Cast<ADestinationActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ADestinationActor::StaticClass()));

	SaveViewportSize(); // 초기 경계 계산
	UpdateIndicatorText(); // 초기 텍스트
}

void UIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float DeltaSeconds)
{
	Super::NativeTick(MyGeometry, DeltaSeconds);

	SaveViewportSize(); // 리사이즈/DPI 반영
	UpdateIndicatorText(); // 거리 갱신
	SetVisibilityIndicator();
}

void UIndicatorWidget::SaveViewportSize()
{
	const FVector2D CurrentViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
	const float CurrentViewportScale = FMath::Max( UWidgetLayoutLibrary::GetViewportScale(this), 0.001f );

	// 이전 상태와 동일하다면 아무 것도 안 함
	if (CurrentViewportSize.Equals(SavedViewportSize, 0.5f) &&
		FMath::IsNearlyEqual(CurrentViewportScale, SavedViewportScale, 0.001f))
	{
		return;
	}

	SavedViewportSize = CurrentViewportSize;
	SavedViewportScale = CurrentViewportScale;
	
	ViewportPx = SavedViewportSize / SavedViewportScale;
	ScreenCenterPx = ViewportPx * 0.5f;

	// 인디케이터 위젯(화살표+텍스트 포함)의 픽셀 크기 추정
	FVector2D IndicatorSize = FVector2D(32.f, 32.f);
	if (OffScreenSizeBox)
	{
		// SizeBox에 명시 크기가 있으면 우선 사용
		const FVector2D Desired = OffScreenSizeBox->GetDesiredSize();
		if (Desired.X > 1.f && Desired.Y > 1.f)
		{
			IndicatorSize = Desired;
		}

		if (UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(OffScreenSizeBox))
		{
			const FVector2D SlotSize = CanvasSlot->GetSize();
			if (SlotSize.X > 1.f && SlotSize.Y > 1.f)
			{
				IndicatorSize = SlotSize;
			}
		}
	}

	const FVector2D Half = IndicatorSize * 0.5f;
	ClampMin = Half + FVector2D(EdgePaddingPx, EdgePaddingPx);
	ClampMax = ViewportPx - Half - FVector2D(EdgePaddingPx, EdgePaddingPx);

	// 안전장치, ClampMin <= ClampMax 보장
	ClampMin.X = FMath::Clamp(ClampMin.X, 0.f, FMath::Max(0.f, ViewportPx.X));
	ClampMin.Y = FMath::Clamp(ClampMin.Y, 0.f, FMath::Max(0.f, ViewportPx.Y));
	ClampMax.X = FMath::Clamp(ClampMax.X, ClampMin.X, ViewportPx.X);
	ClampMax.Y = FMath::Clamp(ClampMax.Y, ClampMin.Y, ViewportPx.Y);
}

void UIndicatorWidget::UpdateIndicatorText()
{
	if (!PC) return;
	APawn* Pawn = PC->GetPawn();
	if (!Pawn || !DestinationActor) return;

	const FVector PlayerLoc = Pawn->GetActorLocation();
	DesLoc = DestinationActor->GetActorLocation();

	// UE 기본: 1 UU = 1 cm → 1 m = 100 UU
	const float DistanceInUU  = FVector::Dist(PlayerLoc, DesLoc);
	const float UnitsPerMeter = FMath::Max(UnrealUnitsPerMeter, 1.f);
	
	const int32 DistanceInMeters = FMath::RoundToInt(DistanceInUU / UnitsPerMeter);
	
	FString Label = LexToString(DistanceInMeters);
	Label.Append(TEXT("m"));
	OffScreenText->SetText(FText::FromString(Label));
}

bool UIndicatorWidget::ProjectWorldToScreenPx(const FVector& WorldPos, FVector2D& OutPx) const
{
	if (!PC) { OutPx = FVector2D::ZeroVector; return false; }

	FVector2D ScreenPosLogical = FVector2D::ZeroVector;
	const bool bOk = UGameplayStatics::ProjectWorldToScreen(PC, WorldPos, ScreenPosLogical, true);

	const float ViewportScale = FMath::Max( UWidgetLayoutLibrary::GetViewportScale(this), 0.001f );
	OutPx = ScreenPosLogical / ViewportScale;
	return bOk;
}

bool UIndicatorWidget::IsInViewportRect(const FVector2D& Px) const
{
	return (Px.X >= 0.f && Px.X <= ViewportPx.X) && (Px.Y >= 0.f && Px.Y <= ViewportPx.Y);
}

FVector2D UIndicatorWidget::BearingToScreenDir2D(const FVector& WorldTarget) const
{
	if (!PC || !PC->PlayerCameraManager) return FVector2D(1.f, 0.f);

	const FVector CamLoc = PC->PlayerCameraManager->GetCameraLocation();
	const FRotator CamRot = PC->PlayerCameraManager->GetCameraRotation();

	const FVector Dir3D = (WorldTarget - CamLoc).GetSafeNormal();
	const float ToYaw = Dir3D.Rotation().Yaw;
	const float DeltaYawDeg = FMath::FindDeltaAngleDegrees(CamRot.Yaw, ToYaw);
	const float R = FMath::DegreesToRadians(DeltaYawDeg);

	// 스크린: +X=오른쪽, +Y=아래
	return FVector2D(FMath::Sin(R), -FMath::Cos(R)).GetSafeNormal();
}

FVector2D UIndicatorWidget::ComputeEdgeIntersection(const FVector2D& StartFrom, const FVector2D& DirNorm) const
{
	// 입력 방향이 거의 0 → 움직이지 않음 → 그냥 박스 안에 Clamp
	if (NearlyZero2D(DirNorm)) return FMath::Clamp(StartFrom, ClampMin, ClampMax);

	auto ComputeHitT = [](float StartCoord, float DirAxis, float BoxMin, float BoxMax)
	{
		// 진행 방향이 거의 0이라면 (해당 축으로 움직이지 않음) → 충돌 없음
		if (FMath::IsNearlyZero(DirAxis)) return TNumericLimits<float>::Max();
		
		// 양수 방향이면 경계의 Max, 음수 방향이면 Min과 만남
		const float TargetBoundary = (DirAxis > 0.f) ? BoxMax : BoxMin;

		// (경계 - 시작좌표) / 진행방향 = 해당 축에서 충돌하는 시점 t
		return (TargetBoundary - StartCoord) / DirAxis;
	};

	const float tHitX = ComputeHitT(StartFrom.X, DirNorm.X, ClampMin.X, ClampMax.X);
	const float tHitY = ComputeHitT(StartFrom.Y, DirNorm.Y, ClampMin.Y, ClampMax.Y);
	float tHit = FMath::Min(tHitX, tHitY);

	// 살짝 안쪽으로 (경계에서 깜빡임 방지)
	tHit = FMath::Max(0.f, tHit) * 0.999f;
	const FVector2D P = StartFrom + DirNorm * tHit;

	return FVector2D(
		FMath::Clamp(P.X, ClampMin.X, ClampMax.X),
		FMath::Clamp(P.Y, ClampMin.Y, ClampMax.Y)
	);
}

void UIndicatorWidget::UpdateIndicatorLocation(bool bOnScreen)
{
	if (!OffScreenSizeBox) return;

	if (bOnScreen)
	{
		OffScreenSizeBox->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	OffScreenSizeBox->SetVisibility(ESlateVisibility::Visible);

	// 위치 반영
	if (UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(OffScreenSizeBox))
	{
		const FVector2D Clamped = FVector2D(
			FMath::Clamp(IndicatorScreenLoc.X, ClampMin.X, ClampMax.X),
			FMath::Clamp(IndicatorScreenLoc.Y, ClampMin.Y, ClampMax.Y)
		);
		CanvasSlot->SetPosition(Clamped);
	}

	// 각도: 중심 → 표시점
	if (OffScreenArrow)
	{
		const FVector2D V = IndicatorScreenLoc - ScreenCenterPx;
		const float Deg = FMath::RadiansToDegrees( FMath::Atan2(V.Y, V.X) );
		OffScreenArrow->SetRenderTransformAngle(Deg + ArrowSpriteCorrect);
	}
}

void UIndicatorWidget::SetVisibilityIndicator()
{
	if (!PC || !DestinationActor) return;

	DesLoc = DestinationActor->GetActorLocation();

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	const float UnitsPerMeter = FMath::Max(UnrealUnitsPerMeter, 1.f);
	const float DistUU = FVector::Dist(Pawn->GetActorLocation(), DesLoc);
	const float DistMeters = DistUU / UnitsPerMeter;

	// 20m 이하일때 숨기자
	if (DistMeters <= HideMeter)
	{
		UpdateIndicatorLocation(true);   // 내부에서 Hidden 처리
		return;
	}

	// 온스크린 판정: 뷰포트 경계 기준
	FVector2D DesPx = FVector2D::ZeroVector;
	const bool bProjected = ProjectWorldToScreenPx(DesLoc, DesPx);
	const bool bInViewport = bProjected && IsInViewportRect(DesPx);

	if (bInViewport)
	{
		// 온스크린이면 숨김
		UpdateIndicatorLocation(true);
		return;
	}

	// 오프스크린 배치: 방위 기반으로 화면 가장자리 교차점 계산
	const FVector2D Dir2D = BearingToScreenDir2D(DesLoc); // 카메라 기준 → 화면 방향
	IndicatorScreenLoc = ComputeEdgeIntersection(ScreenCenterPx, Dir2D);

	UpdateIndicatorLocation(false);
}
