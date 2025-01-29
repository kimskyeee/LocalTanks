#include "TankGameMode.h"

#include "ALightTankCharacter.h"
#include "CoverActorBase.h"
#include "FastLogger.h"
#include "MyPawn.h"
#include "Blueprint/UserWidget.h"
#include "OccupiedZone.h"
#include "OutcomeUI.h"
#include "RespawnManager.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject/Mk/Character/Public/Mk_TankPawn.h"

ATankGameMode::ATankGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = nullptr;

	static ConstructorHelpers::FClassFinder<UOutcomeUI> WBP_PlayerWinnerWidgetClass
	(TEXT("/Game/NewSkye/GameUI/WBP_Victory.WBP_Victory_C"));
	if (WBP_PlayerWinnerWidgetClass.Succeeded())
	{
		PlayerWinnerWidgetClass = WBP_PlayerWinnerWidgetClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UOutcomeUI> WBP_AIWinnerWidgetClass
	(TEXT("/Game/NewSkye/GameUI/WBP_Defeat.WBP_Defeat_C"));
	if (WBP_AIWinnerWidgetClass.Succeeded())
	{
		AIWinnerWidgetClass = WBP_AIWinnerWidgetClass.Class;
	}
}

void ATankGameMode::RespawnTank(ETankRoleID TankRoleID)
{
	if (RespawnManager)
	{
		RespawnManager->RespawnTank(TankRoleID);
	}
}

void ATankGameMode::StartGame()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
	RespawnManager->StartGame();
}

void ATankGameMode::OnWinnerDetected(ETeam WinningTeam)
{
	if (bGameEnded)
	{
		return;
	}
	bGameEnded = true;
	
	switch (WinningTeam)
	{
	case ETeam::Player:
		PlayerWinnerWidget->AddToViewport();
		PlayerWinnerWidget->PlayAnimationFromCode();
		break;
	case ETeam::AI:
		AIWinnerWidget->AddToViewport();
		AIWinnerWidget->PlayAnimationFromCode();
		break;
	default:
		break;	
	}
}

void ATankGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->SetInputMode(FInputModeUIOnly());
	}
	
	AOccupiedZone* OccupiedZone = Cast<AOccupiedZone>(UGameplayStatics::GetActorOfClass(GetWorld(), AOccupiedZone::StaticClass()));
	if (OccupiedZone)
	{
		OccupiedZone->OnWinnerDetectedDelegate.AddDynamic(this, &ATankGameMode::OnWinnerDetected);
	}

	RespawnManager = Cast<ARespawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ARespawnManager::StaticClass()));
	if (RespawnManager)
	{
		RespawnManager->OnAIDefeatedDelegate.AddDynamic(this, &ATankGameMode::OnWinnerDetected);
	}

	PlayerWinnerWidget = Cast<UOutcomeUI>(CreateWidget<UUserWidget>(GetWorld(), PlayerWinnerWidgetClass));
	AIWinnerWidget = Cast<UOutcomeUI>(CreateWidget<UUserWidget>(GetWorld(), AIWinnerWidgetClass));
}
