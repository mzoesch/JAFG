// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class WorldCore : ModuleRules
{
    public WorldCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "JAFGCore",
            "CommonUI",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "CoreUObject",
            "Engine",
        }
        );
    }
}
