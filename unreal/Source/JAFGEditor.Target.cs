// Copyright Epic Games, Inc. All Rights Reserved.
// © 2023 mzoesch. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class JAFGEditorTarget : TargetRules
{
    public JAFGEditorTarget(TargetInfo Target) : base(Target)
    {
        Type                    = TargetType.Editor;
        DefaultBuildSettings    = BuildSettingsVersion.V4;
        IncludeOrderVersion     = EngineIncludeOrderVersion.Unreal5_3;
        ExtraModuleNames.Add("JAFG");
    }
}
