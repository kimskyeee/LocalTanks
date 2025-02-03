#include "RespawnUI.h"

#include "Components/TextBlock.h"

void URespawnUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void URespawnUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bRespawnStarted)
	{
		RespawnTime -= InDeltaTime;
		if (RespawnTime <= 0.f)
		{
			RespawnTime = 0.f;
			bRespawnStarted = false;
		}
		FString TimeStr = FString::Printf(TEXT("%d"), static_cast<int32>(RespawnTime));
		if (RespawnText)
		{
			RespawnText->SetText(FText::FromString(TimeStr));
		}
	}
}

void URespawnUI::UpdateTimer(float Time)
{
	bRespawnStarted = true;
	RespawnTime = Time;
	int32 Sec = static_cast<int32>(RespawnTime);
	if (RespawnText)
	{
		RespawnText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Sec)));
	}
}
