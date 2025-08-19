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

class UCanvasPanelSlot;

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
}

void UMinimapWidget::UpdateList(const TArray<TWeakObjectPtr<AActor>>& List, EMinimapMarkerType Type)
{
	for (const TWeakObjectPtr<AActor>& Ptr : List)
	{
		AActor* Actor = Ptr.Get();
		if (!IsValid(Actor)) continue;

		FVector2D Pixel;
		if (WorldToPixel(Actor->GetActorLocation(), Pixel))
		{
			if (UMinimapMarkerWidget* MarkerWidget = GetOrCreateMarker(Actor, Type))
			{
				MarkerWidget->SetCanvasPosition(Pixel);
				MarkerWidget->SetRotationYaw(Actor->GetActorRotation().Yaw);
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

	if (UMinimapMarkerWidget** Found = MarkerMap.Find(Actor)) return *Found;

	UMinimapMarkerWidget* MarkerWidget = CreateWidget<UMinimapMarkerWidget>(GetWorld(), MarkerWidgetClass);
	if (!MarkerWidget) return nullptr;

	MarkerWidget->InitMarker(Type);
	if (UCanvasPanelSlot* CanvasSlot  = MarkerLayer->AddChildToCanvas(MarkerWidget))
	{
		CanvasSlot ->SetAutoSize(true);
		CanvasSlot ->SetAlignment(FVector2D(0.5f, 0.5f));
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
