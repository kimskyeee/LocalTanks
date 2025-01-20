// Fill out your copyright notice in the Description page of Project Settings.


#include "OccupyUI.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UOccupyUI::NativeConstruct()
{
	Super::NativeConstruct();

	OProgressReset();
}

void UOccupyUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UOccupyUI::OProgressBarGreen(float CurrentP, float TotalP)
{
	float Percent = CurrentP / TotalP;
	OProgressBar_Green->SetPercent(Percent);
	
	OProgressBar_Green->SetVisibility(ESlateVisibility::Visible);

	int32 DisplayPercent = FMath::Clamp(FMath::RoundToInt(Percent * 100.0f), 0, 100);
	FText PercentText = FText::AsNumber(DisplayPercent);
	OingNumText_Green->SetText(PercentText);
	
	if (Percent < 1.0f)
	{
		OingNumText_Green->SetVisibility(ESlateVisibility::Visible);
		OingText_Green->SetVisibility(ESlateVisibility::Visible);
		OSuccessText_Green->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		OingNumText_Green->SetVisibility(ESlateVisibility::Hidden);
		OingText_Green->SetVisibility(ESlateVisibility::Hidden);
		OSuccessText_Green->SetVisibility(ESlateVisibility::Visible);
	}
}

void UOccupyUI::OProgressBarRed(float CurrentP, float TotalP)
{
	float Percent = CurrentP / TotalP;
	OProgressBar_Red->SetPercent(Percent);

	OProgressBar_Red->SetVisibility(ESlateVisibility::Visible);

	int32 DisplayPercent = FMath::Clamp(FMath::RoundToInt(Percent * 100.0f), 0, 100);
	FText PercentText = FText::AsNumber(DisplayPercent);
	OingNumText_Red->SetText(PercentText);
	
	if (Percent < 1.0f)
	{
		OingNumText_Red->SetVisibility(ESlateVisibility::Visible);
		OingText_Red->SetVisibility(ESlateVisibility::Visible);
		OSuccessText_Red->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		OingNumText_Red->SetVisibility(ESlateVisibility::Hidden);
		OingText_Red->SetVisibility(ESlateVisibility::Hidden);
		OSuccessText_Red->SetVisibility(ESlateVisibility::Visible);
	}
}

void UOccupyUI::OProgressReset()
{
	OingText_Green->SetVisibility(ESlateVisibility::Hidden);
	OingText_Red->SetVisibility(ESlateVisibility::Hidden);
	OingNumText_Green->SetVisibility(ESlateVisibility::Hidden);
	OingNumText_Red->SetVisibility(ESlateVisibility::Hidden);
	OSuccessText_Green->SetVisibility(ESlateVisibility::Hidden);
	OSuccessText_Red->SetVisibility(ESlateVisibility::Hidden);

	OProgressBar_Green->SetPercent(0.0f);
	OProgressBar_Red->SetPercent(0.0f);
	OProgressBar_Green->SetVisibility(ESlateVisibility::Hidden);
	OProgressBar_Red->SetVisibility(ESlateVisibility::Hidden);
}
