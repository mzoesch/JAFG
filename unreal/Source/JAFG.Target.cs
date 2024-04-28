// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;

public class JAFGTarget : TargetRules
{
    public JAFGTarget(TargetInfo Target) : base(Target)
    {
        Type                 = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        IncludeOrderVersion  = EngineIncludeOrderVersion.Unreal5_3;

        bWithPushModel = true;

        ExtraModuleNames.Add("JAFG");

        ExtraModuleNames.AddRange(
        new string[]
        {
            "JAFGCore",
            "Menu",
            /* Game ... */
        }
        );
    }
}
