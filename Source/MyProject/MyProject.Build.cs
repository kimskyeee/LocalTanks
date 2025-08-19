// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyProject : ModuleRules
{
	public MyProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "UMG", "ChaosVehicles" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange(new string[]
		{
			"MyProject/deulee/LightTank/Character/Public",
			"MyProject/deulee/LightTank/Character/FSM/Public",
			"MyProject/deulee/LightTank/Character/FSM/State/Public",
			"MyProject/deulee/LightTank/Character/GroundSensor/Public",
			"MyProject/deulee/LightTank/Pathfinding/Public",
			"MyProject/deulee/OccupiedZone/Public",
			"MyProject/deulee/GameMode/Public",
			"MyProject/deulee/CoverActor/Public",
			"MyProject/deulee/CoverComponent/Public",
			"MyProject/deulee/Shell/Public",
			"MyProject/deulee/Armor/Public",
			"MyProject/deulee/Delegate/Public",
			"MyProject/deulee/Utils/Public",
			"MyProject/skyee2/damagesystem/Public",
			"MyProject/skyee2/Character/Public",
			"MyProject/skyee2/Widget/Public",
		});

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
