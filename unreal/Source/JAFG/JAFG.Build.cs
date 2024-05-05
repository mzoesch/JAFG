// Copyright Epic Games, Inc. All Rights Reserved.

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
            "JAFGLogging",
            "JAFGSettings",
            "JAFGSlateCore",
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
    }
}
