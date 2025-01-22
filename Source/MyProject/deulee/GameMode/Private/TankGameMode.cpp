#include "TankGameMode.h"

#include "FastLogger.h"
#include "Blueprint/UserWidget.h"
#include "OccupiedZone.h"
#include "OutcomeUI.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

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

void ATankGameMode::OnWinnerDetected(ETeam WinningTeam)
{
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

	AOccupiedZone* OccupiedZone = Cast<AOccupiedZone>(UGameplayStatics::GetActorOfClass(GetWorld(), AOccupiedZone::StaticClass()));
	if (OccupiedZone)
	{
		OccupiedZone->OnWinnerDetectedDelegate.AddDynamic(this, &ATankGameMode::OnWinnerDetected);
	}

	PlayerWinnerWidget = Cast<UOutcomeUI>(CreateWidget<UUserWidget>(GetWorld(), PlayerWinnerWidgetClass));
	AIWinnerWidget = Cast<UOutcomeUI>(CreateWidget<UUserWidget>(GetWorld(), AIWinnerWidgetClass));
}
