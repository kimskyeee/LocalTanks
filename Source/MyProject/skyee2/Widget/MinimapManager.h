// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapManager.generated.h"

/*
 * 미니맵 좌표계와 대상 목록을 관리하는 액터
 */

UENUM(BlueprintType)
enum class EMinimapMarkerType : uint8
{
	Player       UMETA(DisplayName="Player"), // 플레이어
	AI           UMETA(DisplayName="AI"), // AI 탱크
	Destination  UMETA(DisplayName="Destination") // 목적지
};

UCLASS()
class MYPROJECT_API AMinimapManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMinimapManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap|Bounds")
	FBox2D MapBounds = FBox2D(FVector2D(-20000, -20000), FVector2D(20000, 20000));
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap")
	TWeakObjectPtr<class AMinimapCaptureActor> CapActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap")
	class UTextureRenderTarget2D* RenderTarget = nullptr;

	UFUNCTION()
	void CollectActorsByClass();

	FTimerHandle RecollectHandle;

	// 수집 대상 클래스들 (에디터에서 드롭다운)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap|Classes")
	TSubclassOf<AActor> PlayerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap|Classes")
	TSubclassOf<AActor> AIClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap|Classes")
	TSubclassOf<AActor> DestinationClass;

	// 타입별 읽기
	const TArray<TWeakObjectPtr<AActor>>& GetPlayers() const { return Players; }
	const TArray<TWeakObjectPtr<AActor>>& GetAIs() const { return AIs; }
	const TArray<TWeakObjectPtr<AActor>>& GetDestinations() const { return Destinations; }
	
	// 월드 좌표(WorldLoc)를 미니맵 좌표계로 변환
	UFUNCTION(BlueprintCallable, Category="Minimap|Mapping")
	bool WorldToMinimapUV(const FVector& WorldLoc, FVector2D& OutUV) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap|UI")
	TSubclassOf<class UMinimapWidget> MinimapWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap|UI")
	class UMinimapWidget* MinimapWidget;

private:
	UPROPERTY(EditAnywhere)
	TArray<TWeakObjectPtr<AActor>> Players;
	UPROPERTY(EditAnywhere)
	TArray<TWeakObjectPtr<AActor>> AIs;
	UPROPERTY(EditAnywhere)
	TArray<TWeakObjectPtr<AActor>> Destinations;
};
