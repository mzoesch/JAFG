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
        }
        );

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "JAFGLogging"
        }
        );

        PrivateDependencyModuleNames.AddRange
        (new string[]
        {
            "Slate",
            "SlateCore",
            "UMG",
        }
        );
    }
}
