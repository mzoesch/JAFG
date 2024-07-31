// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolModificationListEntry.h"

#include "JAFGLogDefs.h"
#include "ModificationUBTEditorToolSettings.h"

bool ModificationUBTEditorToolModificationListEntry::MatchesSearchTokens(const IPlugin& Plugin, const TArray<FString>& Tokens)
{
    const FString PluginName   = Plugin.GetName();
    const FString FriendlyName = Plugin.GetDescriptor().FriendlyName;
    const FString Description  = Plugin.GetDescriptor().Description;

    for (const FString& Token : Tokens)
    {
        if (
               PluginName.Contains(Token)
            || FriendlyName.Contains(Token)
            || Description.Contains(Token)
        )
        {
            return true;
        }
    }

    return false;
}

bool ModificationUBTEditorToolModificationListEntry::DoesPluginHaveRuntime(const IPlugin& Plugin)
{
    /* Content plugins always have runtime component. */
    if (Plugin.GetDescriptor().bCanContainContent)
    {
        return true;
    }

    /* C++ plugins have a runtime component as long as one of their modules does. */
    for (const FModuleDescriptor& Module : Plugin.GetDescriptor().Modules)
    {
        if (
               Module.Type == EHostType::Runtime
            || Module.Type == EHostType::RuntimeNoCommandlet
            || Module.Type == EHostType::RuntimeAndProgram
            || Module.Type == EHostType::ClientOnly
            || Module.Type == EHostType::ClientOnlyNoCommandlet
            || Module.Type == EHostType::ServerOnly
            || Module.Type == EHostType::CookedOnly
        )
        {
            return true;
        }
    }

    return false;
}

void SModificationUBTEditorToolModificationListEntry::Construct(const FArguments& InArgs)
{

    this->UpdateModList();
    this->UpdateAllPluginsCheckbox();
    IPluginManager::Get().OnNewPluginCreated().AddSP(this, &SModificationUBTEditorToolModificationListEntry::OnNewPluginCreated);

}

void SModificationUBTEditorToolModificationListEntry::UpdateModList(void)
{
    this->EnabledRuntimePlugins.Empty();

    for (TSharedRef<IPlugin> Plugin : IPluginManager::Get().GetEnabledPlugins())
    {
        if (
               ( this->bShowEnginePlugins  && Plugin->GetType() == EPluginType::Engine  )
            || ( this->bShowProjectPlugins && Plugin->GetType() == EPluginType::Project )
            || Plugin->GetType() == EPluginType::Mod
        )
        {
            if (ModificationUBTEditorToolModificationListEntry::DoesPluginHaveRuntime(Plugin.Get()))
            {
                this->EnabledRuntimePlugins.Add(Plugin);
            }
        }

        continue;
    }

    this->EnabledRuntimePlugins.Sort( [] (const TSharedRef<IPlugin>& A, const TSharedRef<IPlugin>& B)
    {
        return A->GetName() < B->GetName();
    });

    this->FilterModificationsByName(this->GetMostRecentFilter());

    return;
}

void SModificationUBTEditorToolModificationListEntry::FilterModificationsByName(const FString& InFilter)
{
    this->MostRecentFilter = InFilter;
    this->FilteredPlugins.Empty();

    if (InFilter.IsEmpty())
    {
        this->FilteredPlugins = this->EnabledRuntimePlugins;
    }
    else
    {
        TArray<FString> FilterTokens; InFilter.ParseIntoArray(FilterTokens, TEXT(" "), true);

        for (const TSharedRef<IPlugin>& Modification : this->EnabledRuntimePlugins)
        {
            if (ModificationUBTEditorToolModificationListEntry::MatchesSearchTokens(Modification.Get(), FilterTokens))
            {
                this->FilteredPlugins.Add(Modification);
            }
        }
    }

    this->SlatePluginsList->RequestListRefresh();
    this->UpdateAllPluginsCheckbox();

    return;
}

void SModificationUBTEditorToolModificationListEntry::SetAllPluginsToChecked(bool bChecked)
{
    UModificationUBTEditorToolSettings* Settings = UModificationUBTEditorToolSettings::Get();

    for (TSharedRef<IPlugin> Plugin : this->FilteredPlugins)
    {
        TSharedPtr<ITableRow> TableRow = this->SlatePluginsList->WidgetFromItem(Plugin);
        if (!TableRow.IsValid())
        {
            LOG_WARNING(LogModificationUBTEditorTool, "Failed to find table row for plugin %s.", *Plugin->GetName())
            continue;
        }

        TSharedPtr<S>

        Settings->SelectedPlugins.Add(Plugin->GetName(), bChecked);

        continue;
    }

}

void SModificationUBTEditorToolModificationListEntry::UpdateAllPluginsCheckbox(void)
{
    UModificationUBTEditorToolSettings* Settings = UModificationUBTEditorToolSettings::Get();

    bool bAllFalse = true;
    bool bAllTrue  = true;

    for (const TSharedRef<IPlugin>& Modification : this->FilteredPlugins)
    {
        if (Settings->SelectedPlugins.FindOrAdd(Modification->GetName(), false))
        {
            bAllFalse = false;
        }
        else
        {
            bAllTrue = false;
        }

        continue;
    }

    if (bAllTrue == false && bAllFalse == false)
    {
        this->AllPluginsCheckbox->SetIsChecked(ECheckBoxState::Undetermined);
    }
    else if (bAllTrue)
    {
        this->AllPluginsCheckbox->SetIsChecked(ECheckBoxState::Checked);
    }
    else if (bAllFalse)
    {
        this->AllPluginsCheckbox->SetIsChecked(ECheckBoxState::Unchecked);
    }
    else
    {
        checkNoEntry()
    }

    return;
}
