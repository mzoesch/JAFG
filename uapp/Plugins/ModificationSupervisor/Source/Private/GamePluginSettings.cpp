// Copyright 2024 mzoesch. All rights reserved.

#include "GamePluginSettings.h"
#include "JAFGLogDefs.h"

UGamePluginSettings::UGamePluginSettings(void) : Super()
{
    return;
}

void UGamePluginSettings::OnGameBootUp(void)
{
    this->EnabledGamePluginsAtStartup = this->EnabledGamePlugins;

    this->ChangedEnabledGamePlugins.Empty();
    this->AppliedEnabledGamePlugins.Empty();

    return;
}

bool UGamePluginSettings::TogglePluginEnabled(const FString& PluginName)
{
    if (this->ChangedEnabledGamePlugins.Contains(PluginName))
    {
        this->ChangedEnabledGamePlugins[PluginName] = !this->ChangedEnabledGamePlugins[PluginName];
    }
    else
    {
        if (this->EnabledGamePlugins.Contains(PluginName) == false)
        {
            this->EnabledGamePlugins.Add(PluginName, true);
        }
        this->ChangedEnabledGamePlugins.Add(PluginName, this->EnabledGamePlugins[PluginName] == false);
    }

    this->UpdateChangedPluginList(PluginName);

    return this->WillPluginBeEnabled(PluginName);
}

bool UGamePluginSettings::ApplyChangedPluginSettings(void)
{
    if (this->ChangedEnabledGamePlugins.IsEmpty())
    {
        LOG_WARNING(LogModSubsystem, "No changes to apply.")
        return false;
    }

    for (const auto& Plugin : this->ChangedEnabledGamePlugins)
    {
        if (this->EnabledGamePlugins.Contains(Plugin.Key) == false)
        {
            this->EnabledGamePlugins.Add(Plugin.Key, Plugin.Value);
        }
        else
        {
            this->EnabledGamePlugins[Plugin.Key] = Plugin.Value;
        }

        if (this->AppliedEnabledGamePlugins.Contains(Plugin.Key) == false)
        {
            this->AppliedEnabledGamePlugins.Add(Plugin.Key, Plugin.Value);
        }
        else
        {
            this->AppliedEnabledGamePlugins[Plugin.Key] = Plugin.Value;
        }
    }

    this->SaveConfig();

    this->ChangedEnabledGamePlugins.Empty();

    return true;
}

bool UGamePluginSettings::HasPendingChanges(void) const
{
    return this->ChangedEnabledGamePlugins.Num() > 0;
}

bool UGamePluginSettings::IsPluginDivertedFromDefault(const FString& PluginName) const
{
    return this->ChangedEnabledGamePlugins.Contains(PluginName) || this->AppliedEnabledGamePlugins.Contains(PluginName);
}

bool UGamePluginSettings::IsPluginEnabled(const FString& PluginName) const
{
    if (this->EnabledGamePluginsAtStartup.Contains(PluginName))
    {
        return this->EnabledGamePluginsAtStartup[PluginName];
    }

    return false;
}

bool UGamePluginSettings::WillPluginBeEnabled(const FString& PluginName) const
{
    if (this->ChangedEnabledGamePlugins.Contains(PluginName))
    {
        return this->ChangedEnabledGamePlugins[PluginName];
    }

    if (this->AppliedEnabledGamePlugins.Contains(PluginName))
    {
        return this->AppliedEnabledGamePlugins[PluginName];
    }

    return this->IsPluginEnabled(PluginName);
}

bool UGamePluginSettings::WillPluginBeDisabled(const FString& PluginName) const
{
    if (this->ChangedEnabledGamePlugins.Contains(PluginName))
    {
        return this->ChangedEnabledGamePlugins[PluginName] == false;
    }

    if (this->AppliedEnabledGamePlugins.Contains(PluginName))
    {
        return this->AppliedEnabledGamePlugins[PluginName] == false;
    }

    return this->IsPluginEnabled(PluginName) == false;
}

bool UGamePluginSettings::UpdateChangedPluginList(const FString& PluginName)
{
    if (this->ChangedEnabledGamePlugins[PluginName] == this->EnabledGamePluginsAtStartup[PluginName])
    {
        this->ChangedEnabledGamePlugins.Remove(PluginName);
        return true;
    }

    return false;
}
