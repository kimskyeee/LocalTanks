#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RespawnUI.generated.h"

UCLASS()
class URespawnUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void UpdateTimer(float Time);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* RespawnText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRespawnStarted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RespawnTime = 0.f;
};
