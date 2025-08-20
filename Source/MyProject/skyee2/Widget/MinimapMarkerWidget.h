// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinimapManager.h"
#include "Blueprint/UserWidget.h"
#include "MinimapMarkerWidget.generated.h"

enum class EMinimapMarkerType : uint8;
/*
 * 개별 마커의 아이콘/회전 등
 */
UCLASS()
class MYPROJECT_API UMinimapMarkerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Minimap")
	void InitMarker(EMinimapMarkerType InType);
	UFUNCTION(BlueprintCallable, Category="Minimap")
	void SetCanvasPosition(const FVector2D& Pixel);
	UFUNCTION(BlueprintCallable, Category="Minimap")
	void SetRotationYaw(float WorldYaw);

protected:
	virtual void NativeOnInitialized() override;

	
	UPROPERTY(meta=(BindWidget))
	class UImage* IconImage;

	UPROPERTY(EditAnywhere, Category="Minimap|Icon")
	UTexture2D* PlayerIcon;
	UPROPERTY(EditAnywhere, Category="Minimap|Icon")
	UTexture2D* AIIcon;
	UPROPERTY(EditAnywhere, Category="Minimap|Icon")
	UTexture2D* DestinationIcon;


private:
	EMinimapMarkerType Type = EMinimapMarkerType::Player;
};
