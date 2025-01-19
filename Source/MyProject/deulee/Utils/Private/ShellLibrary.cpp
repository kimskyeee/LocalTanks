#include "ShellLibrary.h"

#include "APShell.h"

TMap<EShellID, FShellBasicInfo> UShellLibrary::ShellBasicInfoMap = {
	{ EShellID::Pzgr39, FShellBasicInfo{ EShellID::Pzgr39, 75000, 55.f, 50.f, 0.0f } },
	{ EShellID::Pzgr41, FShellBasicInfo{ EShellID::Pzgr41, 75000, 135.f, 157.f, 0.0f } }
};

TMap<EShellID, UClass*> UShellLibrary::ShellClassMap = {
	{ EShellID::Pzgr39, APShell::StaticClass() },
	{ EShellID::Pzgr41, APShell::StaticClass() }
};

UShellLibrary::UShellLibrary()
{
	// DataTable을 사용하면 더 좋을 수 있음
	// ShellBasicInfoMap.Add(EShellID::Pzgr39, FShellBasicInfo{ EShellID::Pzgr39, 750, 55.f, 50.f });
}

FShellBasicInfo UShellLibrary::GetShellBasicInfo(EShellID ShellID)
{
	if (ShellBasicInfoMap.Contains(ShellID))
	{
		return ShellBasicInfoMap[ShellID];
	}
	else
	{
		return FShellBasicInfo{ EShellID::None, 0.f, 0.f, 0.f };
	}
}

UClass* UShellLibrary::GetShellClass(EShellID ShellID)
{
	return ShellClassMap[ShellID];
}
