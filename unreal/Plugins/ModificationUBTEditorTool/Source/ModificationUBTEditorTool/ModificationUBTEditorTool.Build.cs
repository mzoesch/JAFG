// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class ModificationUBTEditorTool : ModuleRules
{
    public ModificationUBTEditorTool(ReadOnlyTargetRules Target) : base(Target)
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
            "Slate",
            "SlateCore",
            "UnrealEd",
            "PluginBrowser",
            "Projects",
            "InputCore",
        }
        );

        /* JAFG-Plugins */
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
