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

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "CoreUObject",
            "Engine",
        }
        );
    }
}
