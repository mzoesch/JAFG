// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

// ReSharper disable once InconsistentNaming
public class JAFGEditorTarget : TargetRules
{
    public JAFGEditorTarget(TargetInfo Target) : base(Target)
    {
        Type                 = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        IncludeOrderVersion  = EngineIncludeOrderVersion.Unreal5_3;
        ExtraModuleNames.Add("JAFG");
    }
}
