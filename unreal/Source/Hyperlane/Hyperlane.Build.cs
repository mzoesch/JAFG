// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class Hyperlane : ModuleRules
{
    public Hyperlane(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "Sockets",
            "NetCore",
            "Networking",
        }
        );

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFGCore",
            "JAFGNetCore",
        }
        );

        /* Plugins */
        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFGExternalCore",
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
        }
        );
    }
}
