// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class Chat : ModuleRules
{
    public Chat(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "Slate",
            "SlateCore",
            "UMG",
            "InputCore",
            "EnhancedInput",
        }
        );

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFGCore",
            "JAFGSlateCore",
        }
        );

        /* Plugins */
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "CommonJAFGSlate",
            }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "CoreUObject",
            "Engine",
        }
        );

        if (Target.Type == TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "UnrealEd",
            }
            );
        }

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFG",
            "JAFGSettings",
            "JAFGNetCore",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFGGlobalLogging",
        }
        );
    }
}
