// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolModificationList.h"
#include "JAFGLogDefs.h"
#include "ModificationUBTEditorToolEditPluginDialogWidget.h"
#include "ModificationUBTEditorToolSettings.h"

#define LOCTEXT_NAMESPACE "ModificationUBTEditorTool"

bool ModificationUBTEditorToolModificationList::MatchesSearchTokens(const IPlugin& Plugin, const TArray<FString>& Tokens)
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

bool ModificationUBTEditorToolModificationList::DoesPluginHaveRuntime(const IPlugin& Plugin)
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

void SModificationUBTEditorToolModificationListEntry::Construct(const FArguments& Args, TSharedRef<IPlugin> InPlugin, TSharedRef<SModificationUBTEditorToolModificationList> InOwner)
{
    this->ChildSlot[
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot().AutoWidth() [ Args._Lead.Widget ]
        + SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text_Lambda( [InPlugin]
            {
                return FText::FromString(
                    FString::Printf(TEXT("%s (%s)"), *InPlugin->GetDescriptor().FriendlyName, *InPlugin->GetName())
                );
            })
            .HighlightText_Lambda( [InOwner]
            {
                return FText::FromString(InOwner->GetMostRecentFilter());
            })
        ]
        + SHorizontalBox::Slot().AutoWidth() [ Args._Trail.Widget ]
    ];

    return;
}

void SModificationUBTEditorToolModificationList::Construct(const FArguments& InArgs)
{
    this->ChildSlot[
        SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight().Padding(0, 5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 5) [ InArgs._BarSlot.Widget ]
                + SVerticalBox::Slot()
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 5, 0)
                    [
                        SAssignNew(AllPluginsCheckbox, SCheckBox)
                        .OnCheckStateChanged_Lambda( [this] (const ECheckBoxState InState)
                        {
                            this->SetAllPluginsToACheckState(InState == ECheckBoxState::Checked);
                        })
                    ]
                    + SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center).Padding(0, 0, 10, 0)
                    [
                        SNew(SEditableTextBox)
                        .HintText(LOCTEXT("SearchHint", "Search Plugin..."))
                        .OnTextChanged_Lambda( [this] (const FText& InText)
                        {
                            this->FilterModificationsByName(InText.ToString());
                        })
                    ]
                    + SHorizontalBox::Slot().AutoWidth()
                    [
                        SNew(SCheckBox)
                        .Content()
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("ShowEnginePlugins", "Show Engine Plugins"))
                        ]
                        .OnCheckStateChanged_Lambda ( [this] (const ECheckBoxState InState)
                        {
                            this->SetShowEnginePlugins(InState == ECheckBoxState::Checked);
                        })
                    ]
                    + SHorizontalBox::Slot().AutoWidth()
                    [
                        SNew(SSpacer)
                        .Size(FVector2D(10.0f, 10.0f))
                    ]
                    + SHorizontalBox::Slot().AutoWidth()
                    [
                        SNew(SCheckBox)
                        .Content()[
                            SNew(STextBlock)
                            .Text(LOCTEXT("ShowProjectPlugins", "Show Project Plugins"))
                        ]
                        .OnCheckStateChanged_Lambda( [this] (const ECheckBoxState InState)
                        {
                            this->SetShowProjectPlugins(InState == ECheckBoxState::Checked);
                        })
                    ]
                ]
            ]
            + SHorizontalBox::Slot().AutoWidth()
            [
                InArgs._SearchTrail.Widget
            ]
        ]
        + SVerticalBox::Slot().FillHeight(1.0f)
        [
            SNew(SScrollBox)
            .Orientation(Orient_Vertical)
            .ScrollBarAlwaysVisible(true)
            + SScrollBox::Slot()
            [
                SAssignNew(SlatePluginsList, SListView<TSharedRef<IPlugin>>)
                .SelectionMode(ESelectionMode::None)
                .ListItemsSource(&this->FilteredPlugins)
                .OnGenerateRow_Lambda(
                [this, InArgs] (TSharedRef<IPlugin> Mod, const TSharedRef<STableViewBase>& List)
                {
                    UModificationUBTEditorToolSettings* Settings = UModificationUBTEditorToolSettings::Get();
                    return
                        SNew(STableRow<TSharedRef<IPlugin>>, List)
                        .Style(&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.AlternatingRow"))
                        [
                            SNew(SModificationUBTEditorToolModificationListEntry, Mod, SharedThis(this))
                            .Lead()
                            [
                                SNew(SHorizontalBox)
                                + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 5, 0)
                                [
                                    SNew(SCheckBox)
                                    .OnCheckStateChanged_Lambda( [this, Mod] (const ECheckBoxState CheckBoxState)
                                    {
                                        this->OnSpecificPluginCheckboxChanged(Mod, CheckBoxState);
                                    })
                                    .IsChecked_Lambda( [Settings, Mod]
                                    {
                                        return
                                            Settings->SelectedPlugins.FindOrAdd(Mod->GetName(), false)
                                                ? ECheckBoxState::Checked
                                                : ECheckBoxState::Unchecked;
                                    })
                                ]
                                + SHorizontalBox::Slot().AutoWidth()
                                [
                                    InArgs._ModEntryLead.IsBound()
                                        ? InArgs._ModEntryLead.Execute(Mod)
                                        : SNullWidget::NullWidget
                                ]
                                + SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 5, 0).VAlign(VAlign_Center)
                                [
                                    SNew(SButton)
                                    .Text(LOCTEXT("EditMod", "Edit"))
                                    .OnClicked_Lambda( [this, Mod]
                                    {
                                        const TSharedRef<SModificationUBTEditorToolEditPluginDialogWidget> EditModDialog =
                                            SNew(SModificationUBTEditorToolEditPluginDialogWidget, Mod);
                                        FSlateApplication::Get().AddModalWindow(EditModDialog, SharedThis(this));
                                        return FReply::Handled();
                                    })
                                    .ToolTipText_Lambda( [Mod]
                                    {
                                        return FText::FromString(FString::Printf(TEXT("Edit %s via the wizard."), *Mod->GetName()));
                                    })
                                ]
                            ]
                            .Trail()
                            [
                                InArgs._ModEntryTrail.IsBound() ? InArgs._ModEntryTrail.Execute(Mod) : SNullWidget::NullWidget
                            ]
                        ];
                })
            ]
        ]
    ];

    this->UpdatePluginList();
    this->UpdateAllPluginsCheckbox();
    IPluginManager::Get().OnNewPluginCreated().AddSP(this, &SModificationUBTEditorToolModificationList::OnNewPluginCreated);

    return;
}

void SModificationUBTEditorToolModificationList::OnNewPluginCreated(IPlugin& NewPlugin)
{
    this->UpdatePluginList();
}

void SModificationUBTEditorToolModificationList::UpdatePluginList(void)
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
            if (ModificationUBTEditorToolModificationList::DoesPluginHaveRuntime(Plugin.Get()))
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

void SModificationUBTEditorToolModificationList::FilterModificationsByName(const FString& InFilter)
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
            if (ModificationUBTEditorToolModificationList::MatchesSearchTokens(Modification.Get(), FilterTokens))
            {
                this->FilteredPlugins.Add(Modification);
            }
        }
    }

    this->SlatePluginsList->RequestListRefresh();
    this->UpdateAllPluginsCheckbox();

    return;
}

void SModificationUBTEditorToolModificationList::SetAllPluginsToACheckState(const bool bChecked)
{
    UModificationUBTEditorToolSettings* Settings = UModificationUBTEditorToolSettings::Get();

    for (TSharedRef<IPlugin> Plugin : this->FilteredPlugins)
    {
        TSharedPtr<ITableRow> TableRow = this->SlatePluginsList->WidgetFromItem(Plugin);
        if (TableRow.IsValid() == false)
        {
            LOG_WARNING(LogModificationUBTEditorTool, "Failed to find table row for plugin %s.", *Plugin->GetName())
            continue;
        }

        if (
            TSharedPtr<SModificationUBTEditorToolModificationListEntry> PluginEntry
                = StaticCastSharedPtr<SModificationUBTEditorToolModificationListEntry>(TableRow->GetContent());
            PluginEntry.IsValid() == false
        )
        {
            LOG_WARNING(LogModificationUBTEditorTool, "Failed to find plugin entry for plugin %s.", *Plugin->GetName())
            continue;
        }

        Settings->SelectedPlugins.Add(Plugin->GetName(), bChecked);

        continue;
    }

    Settings->SaveSettings();

    return;
}

void SModificationUBTEditorToolModificationList::SetShowEnginePlugins(const bool bInShowEnginePlugins)
{
    this->bShowEnginePlugins = bInShowEnginePlugins;
    this->UpdatePluginList();
    return;
}

void SModificationUBTEditorToolModificationList::SetShowProjectPlugins(const bool bInShowProjectPlugins)
{
    this->bShowProjectPlugins = bInShowProjectPlugins;
    this->UpdatePluginList();
    return;
}

void SModificationUBTEditorToolModificationList::OnSpecificPluginCheckboxChanged(const TSharedRef<IPlugin>& Plugin, const ECheckBoxState NewState)
{
    UModificationUBTEditorToolSettings* Settings = UModificationUBTEditorToolSettings::Get();
    Settings->SelectedPlugins.Add(Plugin->GetName(), NewState == ECheckBoxState::Checked);
    Settings->SaveSettings();

    this->UpdateAllPluginsCheckbox();

    return;
}

void SModificationUBTEditorToolModificationList::UpdateAllPluginsCheckbox(void)
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

#undef LOCTEXT_NAMESPACE
