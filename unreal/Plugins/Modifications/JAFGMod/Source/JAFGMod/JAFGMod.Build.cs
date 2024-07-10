// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class JAFGMod : ModuleRules
{
    public JAFGMod(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
        new string[]
        {
        }
        );

        PrivateIncludePaths.AddRange(
        new string[]
        {
        }
        );

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "DeveloperSettings",
        }
        );

        /* Plugins */
        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFGExternalCore",
            "ModificationSupervisor",
            "CommonJAFGSlate",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "UMG",
        }
        );

        /* Plugins */
        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFGGlobalLogging",
        }
        );

        DynamicallyLoadedModuleNames.AddRange(
        new string[]
        {
        }
        );
    }
}
