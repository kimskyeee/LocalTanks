#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OutcomeUI.generated.h"

UCLASS()
class UOutcomeUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "OutcomeUI")
	void PlayAnimationFromCode();
};
