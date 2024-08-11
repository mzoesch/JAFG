// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GamePluginSettings.generated.h"

/**
 * Never instantiate this class directly.
 * Only use the CDO to access and modify the settings.
 */
UCLASS(Config=JAFG)
class MODIFICATIONSUPERVISOR_API UGamePluginSettings : public UObject
{
    GENERATED_BODY()

public:

    UGamePluginSettings();

    void OnGameBootUp(void);

    /** @return True, if the plugin is enabled, after applied settings *and* restarted. */
    bool TogglePluginEnabled(const FString& PluginName);
    /**
     * @return True, if changes were correctly applied and settings have changed.
     *         This will require a restart of the game to take effect.
     */
    bool ApplyChangedPluginSettings(void);

    bool HasPendingChanges(void) const;

    /** If the plugin state diverts after a restart. */
    bool IsPluginDivertedFromDefault(const FString& PluginName) const;
    /** If it is *now* enabled. */
    bool IsPluginEnabled(const FString& PluginName) const;
    /** If it is enabled *after* we have *restarted* the game. */
    bool WillPluginBeEnabled(const FString& PluginName) const;
    /** If it is disabled *after*, we have *restarted* the game. */
    bool WillPluginBeDisabled(const FString& PluginName) const;

    FORCEINLINE auto GetEnabledGamePlugins(void) const -> const TMap<FString, bool>& { return this->EnabledGamePluginsAtStartup; }
    FORCEINLINE auto GetChangedEnabledGamePlugins(void) const -> const TMap<FString, bool>& { return this->ChangedEnabledGamePlugins; }
    FORCEINLINE auto GetAppliedEnabledGamePlugins(void) const -> const TMap<FString, bool>& { return this->AppliedEnabledGamePlugins; }

private:

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Game Plugin", meta = (AllowPrivateAccess = "true"))
    TMap<FString, bool> EnabledGamePlugins;
    TMap<FString, bool> EnabledGamePluginsAtStartup;

    TMap<FString, bool> ChangedEnabledGamePlugins;
    TMap<FString, bool> AppliedEnabledGamePlugins;

    /**
     * @return True, if plugin was removed.
     *         Meaning, it has now the same settings as the currently applied settings, so no need to make a restart
     *         for this specific plugin.
     */
    bool UpdateChangedPluginList(const FString& PluginName);
};
