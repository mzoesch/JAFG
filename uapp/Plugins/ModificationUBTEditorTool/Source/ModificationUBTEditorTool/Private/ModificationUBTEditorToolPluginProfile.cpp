// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolPluginProfile.h"
#include "PlatformInfo.h"

FString FModificationUBTEditorToolPluginProfile::MakeUATCommandLine(void) const
{
    const FString ProjectPath = FPaths::IsProjectFilePathSet()
    ? FPaths::ConvertRelativePathToFull(FPaths::GetProjectFilePath())
    : FPaths::RootDir() / FApp::GetProjectName() / FApp::GetProjectName() + TEXT(".uproject");

    FString CommandLine =
        FString::Printf(TEXT(
            "-ScriptsForProject=\"%s\" PackagePlugin -project=\"%s\" -clientconfig=%s -serverconfig=%s -utf8output -DLCName=%s"
        ),
            *ProjectPath,
            *ProjectPath,
            LexToString(this->BuildConfiguration),
            LexToString(this->BuildConfiguration),
            *this->PluginName
    );

    if (this->bBuild)
    {
        CommandLine += TEXT(" -build");
    }

    CommandLine += MakeUATPlatformArgs();

    CommandLine += GIsEditor || FApp::IsEngineInstalled() ? TEXT(" -nocompileeditor") : TEXT("");
    CommandLine += FApp::IsEngineInstalled() ? TEXT(" -installed") : TEXT("");

    CommandLine += this->bMergeArchive ? TEXT(" -merge") : TEXT("");

    for (auto& [Platform, GameInfo] : this->PlatformGameInfo)
    {
        if (GameInfo.bCopyToGame)
        {
            CommandLine += FString::Printf(TEXT(" -CopyToGameDirectory_%s=\"%s\""), *Platform, *GameInfo.BinaryGamePath.Path);
        }

        if (GameInfo.bLaunchBinaryGame)
        {
            CommandLine += FString::Printf(TEXT(" -LaunchGame_%s=%s"), *Platform, LexToString(GameInfo.StartGameType));
        }

        if (GameInfo.StartGameType == EModificationUBTEditorToolStartGame::Custom)
        {
            CommandLine += FString::Printf(
                TEXT(" -CustomLaunchPath_%s=\"%s\""),
                *Platform,
                *GameInfo.CustomLaunchPath
            );
            CommandLine += FString::Printf(
                TEXT(" -CustomLaunchArgs_%s=\"%s\""),
                *Platform,
                *GameInfo.CustomLaunchArgs.ReplaceQuotesWithEscapedQuotes()
            );
        }

        continue;
    }

    return CommandLine;
}

FString FModificationUBTEditorToolPluginProfile::ToString(const int32 Indentation /* = 0 */) const
{
    const FString BuildConfigurationString = LexToString(this->BuildConfiguration);

    FString CookedPlatformsString;
    for (const FString& Platform : this->CookedPlatforms)
    {
        CookedPlatformsString += Platform;
        CookedPlatformsString += TEXT(", ");
    }

    FString PlatformGameInfoString = FString::Printf(TEXT("\n%s"), *FString::ChrN(Indentation, TEXT(' ')));
    for (auto& [Platform, GameInfo] : this->PlatformGameInfo)
    {
        PlatformGameInfoString += FString::Printf(
            TEXT("%s%s: {%s}\n%s"),
            *FString::ChrN(4, TEXT(' ')),
            *Platform,
            *GameInfo.ToString(Indentation + 2),
            *FString::ChrN(Indentation, TEXT(' '))
        );
    }

    return FString::Printf(TEXT("%s in %s for [%s] with {%s} (Build: %s, Merge: %s)"),
        *this->PluginName,
        *BuildConfigurationString,
        CookedPlatformsString.IsEmpty() ? TEXT("NO PLATFORMS") : *CookedPlatformsString,
        this->PlatformGameInfo.IsEmpty() ? TEXT("NO PLATFORM GAME INFO") : *PlatformGameInfoString,
        this->bBuild ? TEXT("yes") : TEXT("no"),
        this->bMergeArchive ? TEXT("yes") : TEXT("no")
    );
}

FString FModificationUBTEditorToolPluginProfile::MakeUATPlatformArgs(void) const
{
    FString CommandLine;

    FString ServerCommand   = TEXT("");
    FString ServerPlatforms = TEXT("");
    FString Platforms       = TEXT("");
    FString PlatformCommand = TEXT("");
    FString OptionalParams  = TEXT("");
    TSet<FString> OptionalTargetPlatforms;

    for (const FString& PlatformName : this->CookedPlatforms)
    {
        if (
            const PlatformInfo::FTargetPlatformInfo* PlatformInfo = PlatformInfo::FindPlatformInfo(FName(*PlatformName));
            ensure( PlatformInfo )
        )
        {
            /* Separate out server platforms. */
            FString& PlatformString = (PlatformInfo->PlatformType == EBuildTargetType::Server) ? ServerPlatforms : Platforms;

            PlatformString += TEXT("+");
            PlatformString += PlatformInfo->DataDrivenPlatformInfo->UBTPlatformString;

            /* Append any extra UAT flags specified for this platform flavor. */
            if (PlatformInfo->UATCommandLine.IsEmpty() == false)
            {
                FString OptionalUATCommandLine = PlatformInfo->UATCommandLine;

                OptionalParams += TEXT(" ");
                OptionalParams += OptionalUATCommandLine;
            }
        }

        continue;
    }

    /*
     * If both client and server are desired to be built,
     * avoid server causing clients to not be built PlatformInfo wise.
     */
    if (OptionalParams.Contains(TEXT("-client")) && OptionalParams.Contains(TEXT("-noclient")))
    {
        OptionalParams = OptionalParams.Replace(TEXT("-noclient"), TEXT(""));
    }

    if (ServerPlatforms.Len() > 0)
    {
        ServerCommand = TEXT(" -server -serverplatform=") + ServerPlatforms.RightChop(1);
        if (Platforms.Len() == 0)
        {
            OptionalParams += TEXT(" -noclient");
        }
    }

    if (Platforms.Len() > 0)
    {
        PlatformCommand = TEXT(" -platform=") + Platforms.RightChop(1);
        OptionalParams  = OptionalParams.Replace(TEXT("-noclient"), TEXT(""));
    }

    CommandLine += PlatformCommand;
    CommandLine += ServerCommand;
    CommandLine += OptionalParams;

    return CommandLine;
}
