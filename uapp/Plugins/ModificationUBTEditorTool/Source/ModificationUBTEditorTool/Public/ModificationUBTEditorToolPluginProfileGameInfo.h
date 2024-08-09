// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGLogDefs.h"
#include "ModificationUBTEditorToolSettings.h"

struct FModificationUBTEditorToolPluginProfileGameInfo final
{
    FModificationUBTEditorToolPluginProfileGameInfo(void) = default;

    explicit FModificationUBTEditorToolPluginProfileGameInfo(
        const bool bInCopyToGame
    )
        : bCopyToGame(bInCopyToGame)
    {
        if (bInCopyToGame)
        {
            LOG_FATAL(
                LogModificationUBTEditorTool,
                "FModificationUBTEditorToolPluginProfile: bCopyToGame is true, but no BinaryGamePath was provided"
            )
        }

        return;
    }

    explicit FModificationUBTEditorToolPluginProfileGameInfo(
        const bool bInCopyToGame,
        const FDirectoryPath& InGamePath
    )
        : bCopyToGame(bInCopyToGame)
        , BinaryGamePath(InGamePath)
    {
        return;
    }

    explicit FModificationUBTEditorToolPluginProfileGameInfo(
        const bool bInCopyToGame,
        const FDirectoryPath& InGamePath,
        const bool bInStartGame,
        const EModificationUBTEditorToolStartGame::Type InStartGameType
    )
        : bCopyToGame(bInCopyToGame)
        , BinaryGamePath(InGamePath)
        , bLaunchBinaryGame(bInStartGame)
        , StartGameType(InStartGameType)
    {
        return;
    }

    explicit FModificationUBTEditorToolPluginProfileGameInfo(
        const bool bInCopyToGame,
        const FDirectoryPath& InGamePath,
        const bool bInStartGame,
        const EModificationUBTEditorToolStartGame::Type InStartGameType,
        const FString& InCustomLaunchPath,
        const FString& InCustomLaunchArgs
    )
        : bCopyToGame(bInCopyToGame)
        , BinaryGamePath(InGamePath)
        , bLaunchBinaryGame(bInStartGame)
        , StartGameType(InStartGameType)
        , CustomLaunchPath(InCustomLaunchPath)
        , CustomLaunchArgs(InCustomLaunchArgs)
    {
        return;
    }

    bool bCopyToGame      = false;
    FDirectoryPath BinaryGamePath;
    bool bLaunchBinaryGame = false;
    EModificationUBTEditorToolStartGame::Type StartGameType = EModificationUBTEditorToolStartGame::Type::Steam;

    FString CustomLaunchPath;
    FString CustomLaunchArgs;

    FString ToString(const int32 Indentation = 0) const
    {
        const FString StartGameTypeString = LexToString(this->StartGameType);

        return FString::Printf(
            TEXT("CopyToBin: %s, PathBin: %s, LaunchBin: %s, StartAs: %s, CustomLaunchPath: %s, CustomLaunchArgs: %s"),
            bCopyToGame ? TEXT("yes") : TEXT("no"),
            BinaryGamePath.Path.IsEmpty() ? TEXT("NO PATH") : *BinaryGamePath.Path,
            bLaunchBinaryGame ? TEXT("yes") : TEXT("no"),
            *StartGameTypeString,
            CustomLaunchPath.IsEmpty() ? TEXT("NO PATH") : *CustomLaunchPath,
            CustomLaunchArgs.IsEmpty() ? TEXT("NO ARGS") : *CustomLaunchArgs
        );
    }
};
