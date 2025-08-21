// IndicatorWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IndicatorWidget.generated.h"

UCLASS()
class MYPROJECT_API UIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable)
	void SetVisibilityIndicator();

protected:
	UPROPERTY(meta = (BindWidget))
	class USizeBox* OffScreenSizeBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* OffScreenArrow = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* OffScreenText = nullptr;

	UPROPERTY()
	class APlayerController* PC = nullptr;

	UPROPERTY()
	class ADestinationActor* DestinationActor = nullptr;
	UPROPERTY()
	FVector DesLoc = FVector::ZeroVector;

	// 클램프 사각형과 화면 경계 사이 여백(px)
	UPROPERTY(EditAnywhere, Category="Indicator")
	float EdgePaddingPx = 8.f;

	// 화살표 이미지 위치 보정 우측이 0도일 때는 0, 위쪽이 0도면 90 임
	UPROPERTY(EditAnywhere, Category="Indicator")
	float ArrowSpriteCorrect = 90.f;

	UPROPERTY(EditAnywhere, Category="Indicator")
	float UnrealUnitsPerMeter = 100.f;
	
	// DPI 반영 전 뷰포트 픽셀 크기(슬레이트 단위)
	FVector2D ViewportPx = FVector2D::ZeroVector;

	// 화면 중앙(px, 슬레이트 단위)
	FVector2D ScreenCenterPx = FVector2D::ZeroVector;

	// 인디케이터 표시 좌표(px, 슬레이트 단위) 
	FVector2D IndicatorScreenLoc = FVector2D::ZeroVector;

	// 인디케이터 클램프 범위(px)
	FVector2D ClampMin = FVector2D::ZeroVector;
	FVector2D ClampMax = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, Category="Indicator")
	float HideMeter = 20.f;

private:
	// DPI 스케일을 고려해 ViewportPx/ScreenCenterPx/ClampMin/ClampMax 재계산
	void SaveViewportSize();

	// 거리 텍스트 갱신
	void UpdateIndicatorText();

	// 월드→스크린(px) 투영
	bool ProjectWorldToScreenPx(const FVector& WorldPos, FVector2D& OutPx) const;

	// 스크린 좌표가 뷰포트 내부인지 판단
	bool IsInViewportRect(const FVector2D& Px) const;
	
	FVector2D ComputeEdgeIntersection(const FVector2D& StartFrom, const FVector2D& DirNorm) const;

	// 오프스크린 방향(카메라 기준 방위) → 화면 평면 상 2D 단위 벡터
	FVector2D BearingToScreenDir2D(const FVector& WorldTarget) const;

	// 위치·각도 반영 (OnScreen일 때는 숨기자)
	void UpdateIndicatorLocation(bool bOnScreen);
	
	FVector2D SavedViewportSize = FVector2D::ZeroVector;
	float SavedViewportScale = 1.f;
};
