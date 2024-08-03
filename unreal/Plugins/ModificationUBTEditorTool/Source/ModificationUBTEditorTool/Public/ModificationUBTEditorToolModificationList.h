// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"

class SModificationUBTEditorToolModificationList;

namespace ModificationUBTEditorToolModificationList
{

bool MatchesSearchTokens(const IPlugin& Plugin, const TArray<FString>& Tokens);
bool DoesPluginHaveRuntime(const IPlugin& Plugin);

}

class SModificationUBTEditorToolModificationListEntry final : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SModificationUBTEditorToolModificationListEntry) { }
        SLATE_NAMED_SLOT(FArguments, Lead)
        SLATE_NAMED_SLOT(FArguments, Trail)
    SLATE_END_ARGS()

    auto Construct(const FArguments& Args, TSharedRef<IPlugin> InPlugin, TSharedRef<SModificationUBTEditorToolModificationList> InOwner) -> void;
};

class SModificationUBTEditorToolModificationList final : public SCompoundWidget
{
    DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SWidget>, FOnModEntryLead, const TSharedRef<IPlugin>&)
    DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SWidget>, FOnModEntryTail, const TSharedRef<IPlugin>&)

public:

    SLATE_BEGIN_ARGS(SModificationUBTEditorToolModificationList) { }
        SLATE_NAMED_SLOT(FArguments, BarSlot)
        SLATE_NAMED_SLOT(FArguments, SearchTrail)
        SLATE_EVENT(FOnModEntryLead, ModEntryLead)
        SLATE_EVENT(FOnModEntryTail, ModEntryTrail)
    SLATE_END_ARGS()

    auto Construct(const FArguments& InArgs) -> void;

    auto OnNewPluginCreated(IPlugin& NewPlugin) -> void;
    auto UpdatePluginList(void) -> void;
    auto FilterModificationsByName(const FString& InFilter) -> void;
    auto SetAllPluginsToACheckState(const bool bChecked) -> void;

    auto SetShowEnginePlugins(const bool bInShowEnginePlugins) -> void;
    auto SetShowProjectPlugins(const bool bInShowProjectPlugins) -> void;

    FORCEINLINE auto GetMostRecentFilter(void) const -> FString { return this->MostRecentFilter; }
    FORCEINLINE auto GetFilteredPlugins(void) const -> TArray<TSharedRef<IPlugin>> { return this->FilteredPlugins; }

private:

    auto OnSpecificPluginCheckboxChanged(const TSharedRef<IPlugin>& Plugin, const ECheckBoxState NewState) -> void;
    auto UpdateAllPluginsCheckbox(void) -> void;

    FString MostRecentFilter;
    bool bShowEnginePlugins  = false;
    bool bShowProjectPlugins = false;

    TArray<TSharedRef<IPlugin>> EnabledRuntimePlugins;
    TArray<TSharedRef<IPlugin>> FilteredPlugins;

    TSharedPtr<SListView<TSharedRef<IPlugin>>> SlatePluginsList;
    TSharedPtr<SCheckBox>                      AllPluginsCheckbox;
};
