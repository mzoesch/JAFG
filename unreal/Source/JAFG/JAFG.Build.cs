// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class JAFG : ModuleRules
{
    public JAFG(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "ProceduralMeshComponent",
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

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Chat",
            "Hyperlane",
            "JAFGCore",
            "JAFGSettings",
            "JAFGSlateCore",
            "JAFGNetCore",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "Slate",
            "SlateCore",
            "UMG",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "Sockets",
            "NetCore",
            "Networking",
        }
        );

        PrivateDependencyModuleNames.AddRange(
        new string[]
        {
            "JAFGGlobalLogging",
        }
        );

        if (Target.Type == TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "UnrealEd",
            }
            );
        }
    }
}
