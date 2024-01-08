// Copyright Epic Games, Inc. All Rights Reserved.
// © 2023 mzoesch. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class JAFGTarget : TargetRules
{
    public JAFGTarget(TargetInfo Target) : base(Target)
    {
        Type                    = TargetType.Game;
        DefaultBuildSettings    = BuildSettingsVersion.V4;
        IncludeOrderVersion     = EngineIncludeOrderVersion.Unreal5_3;
        ExtraModuleNames.Add("JAFG");
    }
}
