// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModificationUBTEditorToolPluginProfileGameInfo.h"

struct FModificationUBTEditorToolPluginProfile final
{
    FModificationUBTEditorToolPluginProfile(void) = default;
    explicit FModificationUBTEditorToolPluginProfile(const FString& InPluginName)
        : PluginName(InPluginName)
        , BuildConfiguration(EBuildConfiguration::Shipping)
    {
        return;
    }

    FString PluginName;

    bool bBuild = false;
    EBuildConfiguration BuildConfiguration;
    TArray<FString>     CookedPlatforms;

    TMap<FString, FModificationUBTEditorToolPluginProfileGameInfo> PlatformGameInfo;
    bool bMergeArchive = false;

    auto MakeUATCommandLine(void) const -> FString;

    auto ToString(const int32 Indentation = 0) const -> FString;

private:

    auto MakeUATPlatformArgs(void) const -> FString;
};
