// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;
using System.IO;
using System;

public class JAFGTestModOne : ModuleRules
{
    public JAFGTestModOne(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // JAFG transitive dependencies
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",

            "DeveloperSettings",

            "InputCore",
            "EnhancedInput",

            "SlateCore",
            "Slate",
            "UMG",
        });

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            // ... add public dependencies that you statically link with here ...
        });

        /* JAFG-Plugins */
        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "CommonJAFGSlate",
            "JAFGExternalCore",
            "JAFGGlobalLogging",
            "ModificationSupervisor",
        });

        /* JAFG-Game-Plugins */
        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            // ... add public JAFG plugin dependencies, which your modification uses or expanses on, that you statically link with here ...
        });

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            // ... add private dependencies that you statically link with here ...
        });

        PublicIncludePaths.AddRange(
        new string[]
        {
            // ... add public include paths required here ...
        });

        PrivateIncludePaths.AddRange(
        new string[]
        {
            // ... add private include paths required here ...
        });

        DynamicallyLoadedModuleNames.AddRange(
        new string[]
        {
            // ... add any modules that your module loads dynamically here ...
        });
    }
}
