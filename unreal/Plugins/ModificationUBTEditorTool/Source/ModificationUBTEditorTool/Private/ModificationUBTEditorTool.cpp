// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorTool.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "LevelEditor.h"
#include "ModificationUBTEditorToolCommands.h"
#include "ModificationUBTEditorToolDevelopmentWidget.h"
#include "ModificationUBTEditorToolSettings.h"
#include "ModificationUBTEditorToolShippingWidget.h"

#define LOCTEXT_NAMESPACE "FModificationUBTEditorToolModule"

const FName FModificationUBTEditorToolModule::DevelopmentTabName(TEXT("ModificationUBTEditorToolDevelopment"));
const FName FModificationUBTEditorToolModule::ShippingTabName(TEXT("ModificationUBTEditorToolShipping"));

void FModificationUBTEditorToolModule::StartupModule(void)
{
    this->RegisterSettings();

    FModificationUBTEditorToolCommands::Register();

    this->PluginCommands = MakeShareable(new FUICommandList);
    this->PluginCommands->MapAction(
        FModificationUBTEditorToolCommands::Get().DevelopmentWidget,
        FExecuteAction::CreateLambda( [this] (void)
        {
            FGlobalTabmanager::Get()->TryInvokeTab(this->DevelopmentTabName);
        }),
        FCanExecuteAction()
    );
    this->PluginCommands->MapAction(
        FModificationUBTEditorToolCommands::Get().ShippingWidget,
        FExecuteAction::CreateLambda( [this] (void)
        {
            FGlobalTabmanager::Get()->TryInvokeTab(this->ShippingTabName);
        }),
        FCanExecuteAction()
    );

    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    const TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);
    MenuExtender->AddMenuExtension(
        TEXT("FileProject"),
        EExtensionHook::After,
        this->PluginCommands,
        FMenuExtensionDelegate::CreateLambda( [] (FMenuBuilder& Builder)
        {
            Builder.AddMenuEntry(FModificationUBTEditorToolCommands::Get().DevelopmentWidget);
            Builder.AddMenuEntry(FModificationUBTEditorToolCommands::Get().ShippingWidget);

            return;
        })
    );
    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        this->DevelopmentTabName,
        FOnSpawnTab::CreateLambda( [] (const FSpawnTabArgs& Args)
        {
            const TSharedRef<SModificationUBTEditorToolDevelopmentWidget> Widget = SNew(SModificationUBTEditorToolDevelopmentWidget);
            return SNew(SDockTab).TabRole(ETabRole::NomadTab) [ Widget ];
        }
    ));

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        this->ShippingTabName,
        FOnSpawnTab::CreateLambda( [] (const FSpawnTabArgs& Args)
        {
            const TSharedRef<SModificationUBTEditorToolShippingWidget> Widget = SNew(SModificationUBTEditorToolShippingWidget);
            return SNew(SDockTab).TabRole(ETabRole::NomadTab) [ Widget ];
        }
    ));

    return;
}

void FModificationUBTEditorToolModule::ShutdownModule(void)
{
    if (UObjectInitialized())
    {
        this->UnregisterSettings();
    }

    FModificationUBTEditorToolCommands::Unregister();

    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(this->DevelopmentTabName);
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(this->ShippingTabName);

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void FModificationUBTEditorToolModule::RegisterSettings(void) const
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
    if (SettingsModule == nullptr)
    {
        return;
    }

    const ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer(TEXT("Project"));
    SettingsContainer->DescribeCategory(
        TEXT("Modification UBT Editor Tool"),
        FText::FromString(TEXT("Modification UBT Editor Tool")),
        FText::FromString(TEXT("Settings for the Modification UBT Editor Tool plugin."))
    );

    if (const ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings(
            TEXT("Project"),
            TEXT("Modification UBT Editor Tool"),
            TEXT("General"),
            FText::FromString(TEXT("General")),
            FText::FromString(TEXT("Settings for the Modification UBT Editor Tool plugin.")),
            UModificationUBTEditorToolSettings::Get()
    ); SettingsSection.IsValid())
    {
        SettingsSection->OnModified().BindLambda( [] (void)
        {
            UModificationUBTEditorToolSettings::Get()->SaveSettings();
            return true;
        });
    }

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void FModificationUBTEditorToolModule::UnregisterSettings(void) const
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings")))
    {
        SettingsModule->UnregisterSettings(TEXT("Project"), TEXT("Modification UBT Editor Tool"), TEXT("General"));
    }

    return;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FModificationUBTEditorToolModule, ModificationUBTEditorTool)
