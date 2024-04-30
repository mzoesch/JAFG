// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class Menu : ModuleRules
{
    public Menu(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "JAFGCore",
            "CommonOSD",
            "SessionSupervisor",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "CoreUObject",
            "Engine",
            "SlateCore",
            "Slate",
            "UMG",
        }
        );
    }
}
