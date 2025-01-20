#include "OccupationComponent.h"

UOccupationComponent::UOccupationComponent(): OccupationTime(2.0f)
{
}

void UOccupationComponent::StartOccupationDelayed()
{
	GetWorld()->GetTimerManager().ClearTimer(OccupationTimerHandle);
	bOccupyingAvailable = false;
	TWeakObjectPtr<UOccupationComponent> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(OccupationTimerHandle, FTimerDelegate::CreateLambda([WeakThis]()
	{
		UOccupationComponent* StrongThis = WeakThis.Get();
		StrongThis->StopOccupationDelayed();
	}), OccupationTime, false);
}

void UOccupationComponent::StopOccupationDelayed()
{
	bOccupyingAvailable = true;
}

bool UOccupationComponent::IsOccupyingAvailable()
{
	return bOccupyingAvailable;
}
