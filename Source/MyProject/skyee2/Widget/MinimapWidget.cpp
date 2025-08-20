// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapWidget.h"
#include "MinimapManager.h"
#include "MinimapMarkerWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UMinimapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (MinimapImage && MinimapImageTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(MinimapImageTexture);
		Brush.ImageSize = FVector2D(MinimapImageTexture->SizeX, MinimapImageTexture->SizeY);
		MinimapImage->SetBrush(Brush);
	}

	if (!Manager.IsValid())
	{
		// 매니저 없으면 가져오기
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMinimapManager::StaticClass(), Found);
		if (Found.Num() > 0) Manager = Cast<AMinimapManager>(Found[0]);
	}

	// 주기마다 마커 갱신
	GetWorld()->GetTimerManager().SetTimer(UpdateHandle, this, &UMinimapWidget::UpdateMarkers, UpdateInterval, true);
}

void UMinimapWidget::UpdateMarkers()
{
	if (!Manager.IsValid() || !MarkerLayer) return;

	UpdateList(Manager->GetPlayers(), EMinimapMarkerType::Player);
	UpdateList(Manager->GetAIs(), EMinimapMarkerType::AI);
	UpdateList(Manager->GetDestinations(), EMinimapMarkerType::Destination);

	// 사라진 액터들도 갱신
	// 액터가 죽었을때만 갱신해주는게 가장 좋을 것 같다..
	GarbageCollectInvalidMarkers();
}

void UMinimapWidget::UpdateList(const TArray<TWeakObjectPtr<AActor>>& List, EMinimapMarkerType Type)
{
	for (const TWeakObjectPtr<AActor>& Ptr : List)
	{
		AActor* Actor = Ptr.Get();
		if (!IsValid(Actor)) continue;

		FVector2D Pixel;
		if (WorldToPixelIfInside(Actor->GetActorLocation(), Pixel))
		{
			if (UMinimapMarkerWidget* Marker = GetOrCreateMarker(Actor, Type))
			{
				Marker->SetCanvasPosition(Pixel);
				Marker->SetRotationYaw(Actor->GetActorRotation().Yaw);
				Marker->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}
		else
		{
			// 밖이면 숨김
			if (UMinimapMarkerWidget** Found = MarkerMap.Find(Actor))
			{
				(*Found)->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UMinimapWidget::GarbageCollectInvalidMarkers()
{
	TArray<TWeakObjectPtr<AActor>> ToRemove;
	for (auto& Pair : MarkerMap)
	{
		if (!Pair.Key.IsValid() || !IsValid(Pair.Value)) ToRemove.Add(Pair.Key);
	}
	for (const auto& K : ToRemove)
	{
		if (UMinimapMarkerWidget* MarkerWidget = nullptr; MarkerMap.RemoveAndCopyValue(K, MarkerWidget))
		{
			if (IsValid(MarkerWidget)) MarkerWidget->RemoveFromParent();
		}
	}
}

UMinimapMarkerWidget* UMinimapWidget::GetOrCreateMarker(AActor* Actor, EMinimapMarkerType Type)
{
	if (!MarkerLayer || !MarkerWidgetClass || !IsValid(Actor)) return nullptr;

	// 이미 존재하는 마커 있으면 반환 (중복 생성 방지)
	if (UMinimapMarkerWidget** Found = MarkerMap.Find(Actor)) return *Found;

	// 없으면 새로운 마커를 만들자
	UMinimapMarkerWidget* MarkerWidget = CreateWidget<UMinimapMarkerWidget>(GetWorld(), MarkerWidgetClass);
	if (!MarkerWidget) return nullptr;

	MarkerWidget->InitMarker(Type);
	if (UCanvasPanelSlot* CanvasSlot = MarkerLayer->AddChildToCanvas(MarkerWidget))
	{
		CanvasSlot->SetAutoSize(false);
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		switch (Type)
		{
		case EMinimapMarkerType::Player:
			CanvasSlot->SetSize(FVector2D(PlayerMarkerSize));
			break;
		case EMinimapMarkerType::AI:
			CanvasSlot->SetSize(FVector2D(AIMarkerSize));
			break;
		case EMinimapMarkerType::Destination:
			CanvasSlot->SetSize(FVector2D(DesMarkerSize));
			break;
		}
	}
	MarkerMap.Add(Actor, MarkerWidget);
	return MarkerWidget;
}

bool UMinimapWidget::WorldToPixel(const FVector& World, FVector2D& OutPixel) const
{
	if (!Manager.IsValid() || !MarkerLayer) return false;

	FVector2D UV;
	if (!Manager->WorldToMinimapUV(World, UV)) return false;

	const FVector2D LayerSize = MarkerLayer->GetCachedGeometry().GetLocalSize();
	OutPixel = FVector2D(UV.X * LayerSize.X, (1 - UV.Y) * LayerSize.Y);
	
	return true;
}

bool UMinimapWidget::WorldToPixelIfInside(const FVector& World, FVector2D& OutPixel) const
{
	if (!Manager.IsValid() || !MarkerLayer) return false;

	FVector2D UV;
	if (!Manager->WorldToMinimapUV(World, UV)) return false;

	const FVector2D LayerSize = MarkerLayer->GetCachedGeometry().GetLocalSize();
	if (LayerSize.X <= 0.f || LayerSize.Y <= 0.f) return false;

	// 픽셀/퍼센트 패딩을 UV 기준으로 변환
	const FVector2D PadUV_ByPx  = FVector2D(EdgePaddingPx / LayerSize.X, EdgePaddingPx / LayerSize.Y);
	const FVector2D PadUV = FVector2D(
		FMath::Clamp(PadUV_ByPx.X, 0.f, 0.49f),
		FMath::Clamp(PadUV_ByPx.Y, 0.f, 0.49f)
	);

	// **내부 판정**: 0..1 사이(패딩 고려)일 때만 true
	const bool bInside =
		(UV.X >= 0.f + PadUV.X) && (UV.X <= 1.f - PadUV.X) &&
		(UV.Y >= 0.f + PadUV.Y) && (UV.Y <= 1.f - PadUV.Y);
	if (!bInside) return false;

	// 픽셀 좌표 변환 (여기는 클램프 불필요)
	OutPixel = FVector2D(UV.X * LayerSize.X, (1.f - UV.Y) * LayerSize.Y);
	return true;
}

/* // 캔버스에 비례해서 마커 스케일을 계산할 수 있음
FVector2D UMinimapWidget::GetCanvasSize() const
{
	if (!MarkerLayer) return FVector2D::ZeroVector;

	// MarkerLayer가 캔버스에 붙어 있다면, 해당 Slot의 Layout Size를 가져옴
	if (UCanvasPanelSlot* CanvasSlot  = Cast<UCanvasPanelSlot>(MarkerLayer->Slot))
	{
		const FVector2D SlotSize = CanvasSlot->GetSize();
		if (SlotSize.X > 0.f && SlotSize.Y > 0.f) return SlotSize;
	}

	// MarkerLayer에 Slot 사이즈 정보가 없다면, 뷰포트 기준 사이즈를 fallback으로 사용
	return UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld()).GetLocalSize();
}
*/
