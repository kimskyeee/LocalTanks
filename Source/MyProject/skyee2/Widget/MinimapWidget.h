// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapWidget.generated.h"

enum class EMinimapMarkerType : uint8;
/*
 * 마커를 갱신
 */

UCLASS()
class MYPROJECT_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* MinimapImage;

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* MarkerLayer = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTextureRenderTarget2D* MinimapImageTexture = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap")
	TWeakObjectPtr<class AMinimapManager> Manager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap|Marker")
	TSubclassOf<class UMinimapMarkerWidget> MarkerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap")
	float UpdateInterval = 0.1f; // 업데이트 주기

private:
	FTimerHandle UpdateHandle;

	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, UMinimapMarkerWidget*> MarkerMap;

	UFUNCTION()
	void UpdateMarkers();
	UFUNCTION()
	void UpdateList(const TArray<TWeakObjectPtr<AActor>>& List, EMinimapMarkerType Type);
	UFUNCTION()
	void GarbageCollectInvalidMarkers();

	UFUNCTION()
	UMinimapMarkerWidget* GetOrCreateMarker(AActor* Actor, EMinimapMarkerType Type);
	UFUNCTION()
	bool WorldToPixel(const FVector& World, FVector2D& OutPixel) const;
	// UFUNCTION()
	// FVector2D GetCanvasSize() const;

	UPROPERTY(EditAnywhere)
	float PlayerMarkerSize = 15.f;
	UPROPERTY(EditAnywhere)
	float AIMarkerSize = 10.f;
	UPROPERTY(EditAnywhere)
	float DesMarkerSize = 35.f;
	
	UPROPERTY(EditAnywhere, Category="Minimap|Marker")
	float EdgePaddingPx = 5.f; // 픽셀 패딩

	UFUNCTION()
	bool WorldToPixelIfInside(const FVector& World, FVector2D& OutPixel) const;

};
