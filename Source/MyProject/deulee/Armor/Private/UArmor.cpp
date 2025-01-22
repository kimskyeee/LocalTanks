#include "UArmor.h"

#include "UShellPool.h"
#include "AShell.h"
#include "FastLogger.h"

UArmor::UArmor()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UArmor::BeginPlay()
{
	Super::BeginPlay();

	ShellPool = NewObject<UShellPool>(this, UShellPool::StaticClass());
}

void UArmor::FireShell(EShellID ShellID, FVector Location, FRotator Rotation, FName ProfileName, UClass* TargetClass)
{
	AShell* Shell = ShellPool->AcquireShell(ShellID, this);
	if (!Shell)
	{
		return ;
	}
	Shell->SetTarget(TargetClass);
	Shell->SetActorLocation(Location);
	Shell->SetActorRotation(Rotation);
	Shell->SetCollisionPreset(ProfileName);
	Shell->Active();
}

void UArmor::ReleaseShell(EShellID ShellID, AShell* Shell)
{
	ShellPool->ReleaseShell(ShellID, Shell);
}
