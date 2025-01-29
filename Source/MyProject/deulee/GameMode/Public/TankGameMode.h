#pragma once

#include "CoreMinimal.h"
#include "ETankRoleID.h"
#include "ETeam.h"
#include "GameFramework/GameModeBase.h"
#include "TankGameMode.generated.h"

UCLASS()
class ATankGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ATankGameMode();

	UFUNCTION(BlueprintCallable, Category = "TankGameMode")
	void RespawnTank(ETankRoleID TankRoleID);

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

	UPROPERTY()
	class ARespawnManager* RespawnManager = nullptr;
	UPROPERTY()
	bool bGameEnded = false;
};
