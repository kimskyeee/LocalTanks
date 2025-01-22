#include "UShellPool.h"
#include "AShell.h"
#include "CookerSettings.h"
#include "FastLogger.h"
#include "ShellLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/GarbageCollectionSchema.h"

UShellPool::UShellPool()
{
}

AShell* UShellPool::AcquireShell(EShellID ShellID, UArmor* ArmorRef)
{
	if (Pool.Contains(ShellID))
	{
		if (Pool[ShellID].Shells.Num() > 0)
		{
			AShell* Shell = Pool[ShellID].Shells.Pop();
			Shell->SetArmor(ArmorRef);
			return Shell;
		}
	}
	else
	{
		Pool.Add(ShellID, FShellArray());
	}

	AShell* Shell = GetWorld()->SpawnActor<AShell>(UShellLibrary::GetShellClass(ShellID));
	if (!Shell)
	{
		return nullptr;
	}
	Shell->SetArmor(ArmorRef);
	Shell->SetShellInfo(UShellLibrary::GetShellBasicInfo(ShellID));
	return Shell;
}

void UShellPool::ReleaseShell(EShellID ShellID, AShell* Shell)
{
	Pool[ShellID].Shells.Add(Shell);
}
