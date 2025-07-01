// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class InfiniTwin : ModuleRules
{
	public InfiniTwin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"ECSCore",
			"FlecsLibrary",
			"UIElements",
			"IFC"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Slate",
			"SlateCore",
		});
	}
}
