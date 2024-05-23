// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class JAFGSlateCore : ModuleRules
{
    public JAFGSlateCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "Slate",
            "SlateCore",
            "UMG",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "CoreUObject",
            "Engine",
            "JAFGSettings",
            "InputCore",
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
