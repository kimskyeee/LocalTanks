#pragma once

#include "CoreMinimal.h"
#include "ETeam.h"
#include "GameFramework/GameModeBase.h"
#include "TankGameMode.generated.h"

UCLASS()
class ATankGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ATankGameMode();

protected:
	UFUNCTION()
	void OnWinnerDetected(ETeam WinningTeam);
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TankGameMode")
	TSubclassOf<class UOutcomeUI> PlayerWinnerWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TankGameMode")
	TSubclassOf<class UOutcomeUI> AIWinnerWidgetClass;

	UPROPERTY()
	UOutcomeUI* PlayerWinnerWidget = nullptr;
	UPROPERTY()
	UOutcomeUI* AIWinnerWidget = nullptr;
};
