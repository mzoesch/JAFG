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
            "JAFGCore",
            "JAFGLogging",
            "JAFGSettings",
            "JAFGSlateCore",
            "JAFGNetCore",
            "Hyperlane",
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
            "UnrealEd",
        }
        );
    }
}
