// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class SessionSupervisor : ModuleRules
{
    public SessionSupervisor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "JAFGCore",
        }
        );

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Sockets",
            "NetCore",
            "Networking",
            "OnlineSubsystem",
            "OnlineSubsystemNull",
            "OnlineSubsystemUtils",
            "OnlineSubsystemSteam",
        });

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "CoreUObject",
            "Engine",
        }
        );
    }
}
