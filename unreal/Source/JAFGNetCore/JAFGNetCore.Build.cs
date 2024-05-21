// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class JAFGNetCore : ModuleRules
{
    public JAFGNetCore(ReadOnlyTargetRules Target) : base(Target)
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
            "OnlineSubsystem",
            "OnlineSubsystemNull",
            "OnlineSubsystemUtils",
            "OnlineSubsystemSteam",
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
            "JAFGCore",
            "JAFGLogging",
        }
        );
    }
}
