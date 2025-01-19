#include "TankGameMode.h"

#include "FastLogger.h"
#include "OccupiedZone.h"
#include "Kismet/GameplayStatics.h"

ATankGameMode::ATankGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = nullptr;
}

void ATankGameMode::OnWinnerDetected(ETeam WinningTeam)
{
	FFastLogger::LogScreen(FColor::Red, TEXT("Winner Detected : %d"), (int)WinningTeam);
}

void ATankGameMode::BeginPlay()
{
	Super::BeginPlay();

	AOccupiedZone* OccupiedZone = Cast<AOccupiedZone>(UGameplayStatics::GetActorOfClass(GetWorld(), AOccupiedZone::StaticClass()));
	if (OccupiedZone)
	{
		OccupiedZone->OnWinnerDetectedDelegate.AddDynamic(this, &ATankGameMode::OnWinnerDetected);
	}
}
