#include "TankState.h"

#include "UAStarPathFinding.h"

UTankState::UTankState()
{
	// PathFinding = CreateDefaultSubject<UAStarPathFinding>(TEXT("PathFinding"));
}

void UTankState::Initialize()
{
	PathFinding = NewObject<UAStarPathFinding>(this, UAStarPathFinding::StaticClass());
}
