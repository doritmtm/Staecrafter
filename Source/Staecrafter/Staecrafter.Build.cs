// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Staecrafter : ModuleRules
{
	public Staecrafter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "UMG", "Landscape" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
    }
}
