// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class ModificationSupervisor : ModuleRules
{
    public ModificationSupervisor(ReadOnlyTargetRules Target) : base(Target)
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
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "CoreUObject",
            "Engine",
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
