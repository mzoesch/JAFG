// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ModificationUBTEditorToolSettings.generated.h"

UENUM()
namespace EModificationUBTEditorToolStartGame
{

enum Type : uint8
{
    Steam       UMETA(DisplayName = "Steam"),
    SteamServer UMETA(DisplayName = "Steam (Dedicated Server)"),
    Custom      UMETA(DisplayName = "Custom"),
};

}

MODIFICATIONUBTEDITORTOOL_API auto LexToString(const EModificationUBTEditorToolStartGame::Type InType) -> FString;

USTRUCT()
struct MODIFICATIONUBTEDITORTOOL_API FModificationUBTEditorToolTargetSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Config, Category = Config)
    bool bEnabled = true;

    UPROPERTY(EditAnywhere, Config, Category = Config, meta = (InlineEditConditionToggle))
    bool bCopyModsToGame = false;

    UPROPERTY(EditAnywhere, Config, Category = Config, DisplayName = "Copy to Compiled Game Path", meta = (EditCondition = bCopyModsToGame))
    FDirectoryPath CompiledGamePath;

    UPROPERTY(EditAnywhere, Config, Category = Config, meta = (InlineEditConditionToggle))
    bool bLaunchGame = false;

    UPROPERTY(EditAnywhere, Config, Category = Config, meta = ( EditCondition = bLaunchGame))
    TEnumAsByte<EModificationUBTEditorToolStartGame::Type> LaunchGameType;

    /** After a successful pack, the Unreal Automation tool will execute this. Usefully for custom scripts. */
    UPROPERTY(EditAnywhere, Config, Category = Config, meta = ( EditCondition = "LaunchGameType == EModificationUBTEditorToolStartGame::Custom", EditConditionHides))
    FString CustomLaunchPath;

    /** Arguments for the custom launch path. */
    UPROPERTY(EditAnywhere, Config, Category = Config, meta = ( EditCondition = "LaunchGameType == EModificationUBTEditorToolStartGame::Custom", EditConditionHides))
    FString CustomLaunchArgs;
};

UCLASS(config=JAFGEditor)
class MODIFICATIONUBTEDITORTOOL_API UModificationUBTEditorToolSettings : public UObject
{
    GENERATED_BODY()

public:

    FORCEINLINE static auto Get(void) -> UModificationUBTEditorToolSettings*
    {
        return GetMutableDefault<UModificationUBTEditorToolSettings>();
    }

    void SaveSettings(void);

    UPROPERTY(EditAnywhere, Config, Category = "Dev Packaging Settings", DisplayName = "Windows")
    FModificationUBTEditorToolTargetSettings WindowsGameTargetSettings;

    UPROPERTY(EditAnywhere, Config, Category = "Dev Packaging Settings", DisplayName="Windows Server")
    FModificationUBTEditorToolTargetSettings WindowsServerTargetSettings;

    UPROPERTY(EditAnywhere, Config, Category = "Dev Packaging Settings", DisplayName="Linux Server")
    FModificationUBTEditorToolTargetSettings LinuxServerTargetSettings;

    UPROPERTY(BlueprintReadOnly, Config, Category="Config")
    TMap<FString, bool> SelectedPlugins;
};
