// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class CommonJAFGSlate : ModuleRules
{
    public CommonJAFGSlate(ReadOnlyTargetRules Target) : base(Target)
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
            "Slate",
            "SlateCore",
            "UMG",
        }
        );

        /* Plugins */
        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFGExternalCore",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "CoreUObject",
            "Engine",
            "InputCore",
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
