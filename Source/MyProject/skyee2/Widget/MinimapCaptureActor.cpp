// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapCaptureActor.h"

#include "Components/SceneCaptureComponent2D.h"


// Sets default values
AMinimapCaptureActor::AMinimapCaptureActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCaptureComponent2D()->ProjectionType = ECameraProjectionMode::Orthographic;
	GetCaptureComponent2D()->OrthoWidth = 5000.f;
	GetCaptureComponent2D()->bCaptureEveryFrame = false;
	GetCaptureComponent2D()->bCaptureOnMovement = false;
	GetCaptureComponent2D()->CaptureSource = SCS_FinalColorLDR;	
}

// Called when the game starts or when spawned
void AMinimapCaptureActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMinimapCaptureActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMinimapCaptureActor::TakeOneShotCapture(bool bDisableAfter)
{
	if (!GetCaptureComponent2D() || !MinimapCapture) return;

	GetCaptureComponent2D()->TextureTarget = MinimapCapture;
	GetCaptureComponent2D()->CaptureScene();

	UE_LOG(LogTemp, Warning, TEXT("Minimap Capture Complete"));
	
	if (bDisableAfter)
	{
		// 비용 최소화
		SetActorHiddenInGame(true);
		SetActorTickEnabled(false);
		GetCaptureComponent2D()->Deactivate();
	}
}

