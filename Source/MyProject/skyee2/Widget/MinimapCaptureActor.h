// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "MinimapCaptureActor.generated.h"

/*
 * 초기 미니맵을 캡쳐하기 위한 액터
 * 월드 상에 배치해 둘 것!
 */

UCLASS()
class MYPROJECT_API AMinimapCaptureActor : public ASceneCapture2D
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMinimapCaptureActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TakeOneShotCapture(bool bDisableAfter);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SceneCapture")
	class UTextureRenderTarget2D* MinimapCapture = nullptr;
};
