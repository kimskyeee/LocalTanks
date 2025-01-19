#include "UArmor.h"

#include "UShellPool.h"
#include "AShell.h"

UArmor::UArmor()
{
	PrimaryComponentTick.bCanEverTick = false;

	ShellPool = CreateDefaultSubobject<UShellPool>(TEXT("ShellPool"));
}

void UArmor::FireShell(EShellID ShellID, FVector Location, FRotator Rotation, FName ProfileName)
{
	AShell* Shell = ShellPool->AcquireShell(ShellID, this);
	if (!Shell)
	{
		return ;
	}
	Shell->SetActorLocation(Location);
	Shell->SetActorRotation(Rotation);
	Shell->SetCollisionPreset(ProfileName);
	Shell->Active();
}

void UArmor::ReleaseShell(EShellID ShellID, AShell* Shell)
{
	ShellPool->ReleaseShell(ShellID, Shell);
}
