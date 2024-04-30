// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class JAFG : ModuleRules
{
    public JAFG(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
        }
        );

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFGCore",
            "Menu",
            "WorldCore",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
        }
        );

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
