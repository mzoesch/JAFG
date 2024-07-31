// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolDevelopmentWidget.h"

#include "JAFGLogDefs.h"
#include "ModificationUBTEditorTool.h"
#include "ModificationUBTEditorToolSettings.h"

#define LOCTEXT_NAMESPACE "ModificationUBTEditorTool"

void SModificationUBTEditorToolDevelopmentWidget::Construct(const FArguments& InArgs)
{
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bAllowSearch                  = true;
    DetailsViewArgs.bHideSelectionTip             = true;
    DetailsViewArgs.bLockable                     = false;
    DetailsViewArgs.bSearchInitialKeyFocus        = true;
    DetailsViewArgs.bUpdatesFromSelection         = false;
    DetailsViewArgs.bShowOptions                  = true;
    DetailsViewArgs.bShowModifiedPropertiesOption = true;
    DetailsViewArgs.bShowObjectLabel              = false;
    DetailsViewArgs.bCustomNameAreaLocation       = true;
    DetailsViewArgs.bCustomFilterAreaLocation     = true;
    DetailsViewArgs.NameAreaSettings              = FDetailsViewArgs::HideNameArea;
    DetailsViewArgs.bAllowMultipleTopLevelObjects = true;
    DetailsViewArgs.bShowScrollBar                = false;

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
    this->DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    this->DetailsView->SetObject(UModificationUBTEditorToolSettings::Get());
    this->DetailsView->OnFinishedChangingProperties().AddLambda( [this] (const FPropertyChangedEvent& PropertyChangedEvent)
    {
        UModificationUBTEditorToolSettings::Get()->SaveSettings();
    });

    this->ChildSlot
    [
        SNew(SVerticalBox)
        +SVerticalBox::Slot().AutoHeight()
        [
            this->DetailsView.ToSharedRef()
        ]
        +SVerticalBox::Slot().FillHeight(1).Padding(3)
        [
            SAssignNew(this->PluginList, SModificationUBTEditorToolModificationList)
            .BarSlot()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth()
                [
                    SNew(SButton)
                    .Text(LOCTEXT("PackagePluginAllDevelopment", "Package Selected (Development)"))
                    .OnClicked_Lambda( [this]
                    {
                        return this->PackageAllMods();
                    })
                    .IsEnabled(this, &SModificationUBTEditorToolDevelopmentWidget::IsPackageButtonEnabled)
                ]
                + SHorizontalBox::Slot().FillWidth(1.0f)
                + SHorizontalBox::Slot().AutoWidth()
                [
                    SNew(SButton)
                    .Text(LOCTEXT("CreatePlugin", "Create Plugin"))
                    .OnClicked(this, &SModificationUBTEditorToolDevelopmentWidget::CreatePlugin)
                ]
            ]
            .ModEntryLead_Lambda( [this] (const TSharedRef<IPlugin>& Plugin)
            {
                return
                    SNew(SButton)
                    .Text(LOCTEXT("PackagePluginSelectedDevelopment", "Package"))
                    .OnClicked_Lambda( [this, Plugin]
                    {
                        FModificationUBTEditorToolModule::Get().PackagePluginsDevelopment( { Plugin } );
                        return FReply::Handled();
                    })
                    .IsEnabled_Lambda( [this]
                    {
                        return FModificationUBTEditorToolModule::Get().IsPackaging() == false;
                    });
            })
        ]
    ];

    return;
}

FReply SModificationUBTEditorToolDevelopmentWidget::PackageAllMods(void) const
{
    FModificationUBTEditorToolModule&   Module   = FModificationUBTEditorToolModule::Get();
    UModificationUBTEditorToolSettings* Settings = UModificationUBTEditorToolSettings::Get();

    TArray<TSharedRef<IPlugin>> PluginsToPackage;
    for (TSharedRef<IPlugin> Plugin : this->PluginList->GetFilteredPlugins())
    {
        if (Settings->SelectedPlugins.FindOrAdd(Plugin->GetName(), false))
        {
            PluginsToPackage.Add(Plugin);
        }
    }

    Module.PackagePluginsDevelopment(PluginsToPackage);

    return FReply::Handled();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
FReply SModificationUBTEditorToolDevelopmentWidget::CreatePlugin(void) const
{
    LOG_WARNING(LogModificationUBTEditorTool, "Not implemented yet.")
    return FReply::Handled();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool SModificationUBTEditorToolDevelopmentWidget::IsPackageButtonEnabled(void) const
{
    return FModificationUBTEditorToolModule::Get().IsPackaging() == false;
}
