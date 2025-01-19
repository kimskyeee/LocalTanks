#include "TankState.h"

#include "UAStarPathFinding.h"

UTankState::UTankState()
{
	PathFinding = CreateDefaultSubobject<UAStarPathFinding>(TEXT("PathFinding"));
}
