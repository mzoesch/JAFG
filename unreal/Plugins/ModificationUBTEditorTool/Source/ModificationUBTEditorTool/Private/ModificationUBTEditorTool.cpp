// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorTool.h"

#include "GeneralProjectSettings.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "LevelEditor.h"
#include "IPluginBrowser.h"
#include "JAFGLogDefs.h"
#include "ModificationUBTEditorToolCommands.h"
#include "ModificationUBTEditorToolDevelopmentWidget.h"
#include "ModificationUBTEditorToolSettings.h"
#include "ModificationUBTEditorToolShippingWidget.h"

#define LOCTEXT_NAMESPACE "FModificationUBTEditorToolModule"

const FName FModificationUBTEditorToolModule::DevelopmentTabName(TEXT("ModificationUBTEditorToolDevelopment"));
const FName FModificationUBTEditorToolModule::ShippingTabName(TEXT("ModificationUBTEditorToolShipping"));
const FName FModificationUBTEditorToolModule::PluginCreatorTabName(TEXT("ModificationUBTEditorToolPluginCreator"));

void FModificationUBTEditorToolModule::StartupModule(void)
{
    this->RegisterSettings();
    this->RegisterPluginTemplates();

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
        FMenuExtensionDelegate::CreateLambda( [] (FMenuBuilder& InBuilder)
        {
            InBuilder.AddMenuEntry(FModificationUBTEditorToolCommands::Get().DevelopmentWidget);
            InBuilder.AddMenuEntry(FModificationUBTEditorToolCommands::Get().ShippingWidget);

            return;
        })
    );
    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        this->DevelopmentTabName,
        FOnSpawnTab::CreateLambda( [] (const FSpawnTabArgs& InArgs)
        {
            const TSharedRef<SModificationUBTEditorToolDevelopmentWidget> Widget = SNew(SModificationUBTEditorToolDevelopmentWidget);
            return SNew(SDockTab).TabRole(ETabRole::NomadTab) [ Widget ];
        })
    );

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        this->ShippingTabName,
        FOnSpawnTab::CreateLambda( [] (const FSpawnTabArgs& InArgs)
        {
            const TSharedRef<SModificationUBTEditorToolShippingWidget> Widget = SNew(SModificationUBTEditorToolShippingWidget);
            return SNew(SDockTab).TabRole(ETabRole::NomadTab) [ Widget ];
        })
    );

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        this->PluginCreatorTabName,
        FOnSpawnTab::CreateLambda( [] (const FSpawnTabArgs& InArgs)
        {
            IPluginBrowser& PluginBrowser = FModuleManager::Get().GetModuleChecked<IPluginBrowser>(TEXT("PluginBrowser"));
            return PluginBrowser.SpawnPluginCreatorTab(InArgs, MakeShared<FModificationUBTEditorToolPluginWizardDefinition>());
        })
    );

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
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(this->PluginCreatorTabName);

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
FString FModificationUBTEditorToolModule::GetCurrentGameVersion(void)
{
    /*
     * This does only work for me, as I have the source code of the game. For distributed builds, we
     * have to go to the binary version where this plugin is referenced to.
     * UModificationUBTEditorToolSettings should be used for this.
     * We have to solve this differently. For example, through the installed
     * game version (inside steamapps/common/...).
     */
    return GetDefault<UGeneralProjectSettings>()->ProjectVersion;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
FString FModificationUBTEditorToolModule::GetEngineVersion(void)
{
    /*
     * This does only work for me, as I have the source code of the game. For distributed builds, we
     * have to go to the binary version where this plugin is referenced to.
     * UModificationUBTEditorToolSettings should be used for this.
     * We have to solve this differently. For example, through the installed
     * game version (inside steamapps/common/...).
     */
    FString EngineVersion = FApp::GetBuildVersion();
    return EngineVersion;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
FString FModificationUBTEditorToolModule::GetVersionOfCorePlugin(const FString& CorePluginName)
{
    /*
     * This does only work for me, as I have the source code of the game. For distributed builds, we
     * have to go to the binary version where this plugin is referenced to.
     * UModificationUBTEditorToolSettings should be used for this.
     * We have to solve this differently. For example, through the installed
     * game version (inside steamapps/common/...).
     */
    if (const TSharedPtr<IPlugin> CorePlugin= IPluginManager::Get().FindPlugin(CorePluginName); CorePlugin)
    {
        return CorePlugin->GetDescriptor().VersionName;
    }

    return TEXT("-1");
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool FModificationUBTEditorToolModule::IsPluginConsideredAsModification(const IPlugin& Plugin) const
{
    if (Plugin.GetType() == EPluginType::Mod)
    {
        return true;
    }

    if (Plugin.GetType() == EPluginType::Project)
    {
        return true;
    }

    return false;
}

void FModificationUBTEditorToolModule::PackagePluginsDevelopment(TArray<TSharedRef<IPlugin>> Plugins)
{
}

void FModificationUBTEditorToolModule::PackagePluginsShipping(TArray<TSharedRef<IPlugin>> Plugins)
{
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

void FModificationUBTEditorToolModule::RegisterPluginTemplates(void)
{
    for (TSharedRef<IPlugin> Plugin : IPluginManager::Get().GetEnabledPlugins())
    {
        if (this->IsPluginConsideredAsModification(Plugin.Get()))
        {
            this->AddPluginTemplatesFromPlugin(Plugin.Get());
        }
    }

    IPluginManager::Get().OnNewPluginMounted().AddLambda( [this] (const IPlugin& Plugin)
    {
        if (this->IsPluginConsideredAsModification(Plugin))
        {
            this->AddPluginTemplatesFromPlugin(Plugin);
        }
    });

    return;

}

void FModificationUBTEditorToolModule::AddPluginTemplatesFromPlugin(const IPlugin& Plugin)
{
    const FString TemplatesPath = Plugin.GetBaseDir() / TEXT("Templates");
    if (FPaths::DirectoryExists(TemplatesPath) == false)
    {
        return;
    }

    const FString TemplatesJSONPath = TemplatesPath / TEXT("templates.json");
    if (FPaths::FileExists(TemplatesJSONPath) == false)
    {
        return;
    }

    FString TemplatesJSON;
    if (FFileHelper::LoadFileToString(TemplatesJSON, *TemplatesJSONPath) == false)
    {
        return;
    }

    const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(TemplatesJSON);
    TSharedPtr<FJsonObject> TemplatesJSONObj;
    if (FJsonSerializer::Deserialize(JsonReader, TemplatesJSONObj) == false)
    {
        LOG_ERROR(LogModificationUBTEditorTool, "Invalid templates for plugin %s: invalid json.", *Plugin.GetName());
        return;
    }

    const TArray<TSharedPtr<FJsonValue>>* TemplatesArrayPtr;
    if (TemplatesJSONObj->TryGetArrayField(TEXT("templates"), TemplatesArrayPtr) == false)
    {
        LOG_ERROR(LogModificationUBTEditorTool, "Invalid templates for plugin %s: \"templates\" not in json.", *Plugin.GetName());
        return;
    }

    int i = 0;
    for (const TSharedPtr<FJsonValue> Item : *TemplatesArrayPtr)
    {
        ++i;

        const TSharedPtr<FJsonObject>* ItemObjPtr;
        if (Item->TryGetObject(ItemObjPtr) == false)
        {
            LOG_ERROR(LogModificationUBTEditorTool, "Invalid templates for plugin %s: template %d: not an object.", *Plugin.GetName(), i);
            continue;
        }

        FString Error;
        TSharedPtr<FMyPluginTemplateDescription> ModTemplate =
            FMyPluginTemplateDescription::Load(*ItemObjPtr, TemplatesPath, Error);
        if (ModTemplate.IsValid() == false)
        {
            LOG_ERROR(LogModificationUBTEditorTool, "Invalid templates for plugin %s: template %d: [%s].", *Plugin.GetName(), i, *Error);
            continue;
        }

        this->PluginTemplateDefinitions.Add(ModTemplate.ToSharedRef());
    }

    return;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FModificationUBTEditorToolModule, ModificationUBTEditorTool)
