// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapMarkerWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UMinimapMarkerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InitMarker(Type);
}

void UMinimapMarkerWidget::InitMarker(const EMinimapMarkerType InType)
{
	Type = InType;
	if (!IconImage) return;

	UTexture2D* Icon = nullptr;
	switch (Type)
	{
	case EMinimapMarkerType::Player:
		Icon = PlayerIcon;
		break;
	case EMinimapMarkerType::AI:
		Icon = AIIcon;
		break;
	case EMinimapMarkerType::Destination:
		Icon = DestinationIcon;
		break;
	}

	if (Icon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Icon);
		IconImage->SetBrush(Brush);
	}
}

void UMinimapMarkerWidget::SetCanvasPosition(const FVector2D& Pixel)
{
	if (UCanvasPanelSlot* SetSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		SetSlot->SetPosition(Pixel);
	}
}

void UMinimapMarkerWidget::SetRotationYaw(float WorldYaw)
{
	SetRenderTransformAngle(WorldYaw);
}
