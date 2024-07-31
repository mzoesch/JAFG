// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"

namespace ModificationUBTEditorToolModificationListEntry
{

bool MatchesSearchTokens(const IPlugin& Plugin, const TArray<FString>& Tokens);
bool DoesPluginHaveRuntime(const IPlugin& Plugin);

}

class S

class SModificationUBTEditorToolModificationListEntry final : public SCompoundWidget
{
    DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SWidget>, FOnModEntryLead, const TSharedRef<IPlugin>&)
    DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SWidget>, FOnModEntryTail, const TSharedRef<IPlugin>&)

public:

    SLATE_BEGIN_ARGS(SModificationUBTEditorToolModificationListEntry) { }
        SLATE_NAMED_SLOT(FArguments, BarSlot)
        SLATE_NAMED_SLOT(FArguments, SearchTrail)
        SLATE_EVENT(FOnModEntryLead, ModEntryLead)
        SLATE_EVENT(FOnModEntryTail, ModEntryTrail)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void UpdateModList(void);
    void FilterModificationsByName(const FString& InFilter);

    void SetAllPluginsToChecked(bool bChecked);

    FORCEINLINE auto GetMostRecentFilter(void) const -> FString { return this->MostRecentFilter; }

private:

    void UpdateAllPluginsCheckbox(void);

    FString MostRecentFilter;
    bool bShowEnginePlugins  = false;
    bool bShowProjectPlugins = false;

    TArray<TSharedRef<IPlugin>> EnabledRuntimePlugins;
    TArray<TSharedRef<IPlugin>> FilteredPlugins;

    TSharedPtr<SListView<TSharedRef<IPlugin>>> SlatePluginsList;
    TSharedPtr<SCheckBox> AllPluginsCheckbox;
};
