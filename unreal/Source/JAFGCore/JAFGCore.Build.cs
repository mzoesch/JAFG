// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class JAFGCore : ModuleRules
{
    public JAFGCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFGLogging",
        }
        );
    }
}
