#include "HPWidget.h"

#include "FastLogger.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHPWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
}

void UHPWidget::SetName(const FString& Str)
{
	TextBlock_24->SetText(FText::FromString(Str));
}

void UHPWidget::UpdateHPProgessBar(float CurrHP, float MaxHP)
{
	float Percent = CurrHP / MaxHP;
	FFastLogger::LogScreen(FColor::Green, TEXT("HP : %f"), Percent);
	ProgressBar_HP->SetPercent(Percent);
}
