// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class CommonOSD : ModuleRules
{
    public CommonOSD(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "JAFGCore",
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
        }
        );
    }
}
