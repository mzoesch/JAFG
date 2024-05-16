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
            "JAFGLogging",
            "JAFGSlateCore",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "CoreUObject",
            "Engine",
            "UnrealEd",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFG",
            "JAFGSettings",
        }
        );
    }
}
