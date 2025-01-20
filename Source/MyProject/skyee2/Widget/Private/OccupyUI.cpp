// Fill out your copyright notice in the Description page of Project Settings.


#include "OccupyUI.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UOccupyUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (OingText_Green) OingText_Green->SetVisibility(ESlateVisibility::Collapsed);
	if (OingText_Red) OingText_Red->SetVisibility(ESlateVisibility::Collapsed);
	if (OingNumText_Green) OingNumText_Green->SetVisibility(ESlateVisibility::Collapsed);
	if (OingNumText_Red) OingNumText_Red->SetVisibility(ESlateVisibility::Collapsed);
	if (OSuccessText_Green) OSuccessText_Green->SetVisibility(ESlateVisibility::Collapsed);
	if (OSuccessText_Red) OSuccessText_Red->SetVisibility(ESlateVisibility::Collapsed);
}

void UOccupyUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UOccupyUI::OProgressBarGreen(float CurrentP, float TotalP)
{
	float Percent = CurrentP / TotalP;
	OProgressBar_Green->SetPercent(Percent);

	int32 DisplayPercent = FMath::Clamp(FMath::RoundToInt(Percent * 100.0f), 0, 100);
	FText PercentText = FText::AsNumber(DisplayPercent);
	OingNumText_Green->SetText(PercentText);
	
	if (Percent < 1.0f)
	{
		OingNumText_Green->SetVisibility(ESlateVisibility::Visible);
		OingText_Green->SetVisibility(ESlateVisibility::Visible);
		OSuccessText_Green->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		OingNumText_Green->SetVisibility(ESlateVisibility::Collapsed);
		OingText_Green->SetVisibility(ESlateVisibility::Collapsed);
		OSuccessText_Green->SetVisibility(ESlateVisibility::Visible);
	}
}

void UOccupyUI::OProgressBarRed(float CurrentP, float TotalP)
{
	float Percent = CurrentP / TotalP;
	OProgressBar_Red->SetPercent(Percent);

	int32 DisplayPercent = FMath::Clamp(FMath::RoundToInt(Percent * 100.0f), 0, 100);
	FText PercentText = FText::AsNumber(DisplayPercent);
	OingNumText_Red->SetText(PercentText);
	
	if (Percent < 1.0f)
	{
		OingNumText_Red->SetVisibility(ESlateVisibility::Visible);
		OingText_Red->SetVisibility(ESlateVisibility::Visible);
		OSuccessText_Red->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		OingNumText_Red->SetVisibility(ESlateVisibility::Collapsed);
		OingText_Red->SetVisibility(ESlateVisibility::Collapsed);
		OSuccessText_Red->SetVisibility(ESlateVisibility::Visible);
	}
	
}
