// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class JAFGSettings : ModuleRules
{
    public JAFGSettings(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "DeveloperSettings",
            "JAFGSlateCore",
        }
        );

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "CommonSettings",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "CoreUObject",
            "Engine",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFGGlobalLogging",
            "JAFGCore",
        }
        );
    }
}
