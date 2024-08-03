// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationUBTEditorToolPluginWizardDefinition.h"
#include "DOM/JsonObject.h"
#include "JAFGLogDefs.h"
#include "ModificationUBTEditorTool.h"
#include "Features/IPluginsEditorFeature.h"
#include "Interfaces/IPluginManager.h"

FModificationUBTEditorToolPluginWizardDefinition::FModificationUBTEditorToolPluginWizardDefinition(void)
{
    this->PluginBaseDir = IPluginManager::Get().FindPlugin(TEXT("ModificationUBTEditorTool"))->GetBaseDir();

    this->PluginTemplateDefinitions = FModificationUBTEditorToolModule::Get().GetPluginTemplates();
    for (const TSharedRef<FMyPluginTemplateDescription>& Template : this->PluginTemplateDefinitions)
    {
        this->TemplateDefinitions.Add(Template->TemplateDescription);
    }

    return;
}

const TArray<TSharedRef<FPluginTemplateDescription>>& FModificationUBTEditorToolPluginWizardDefinition::GetTemplatesSource(void) const
{
    return this->TemplateDefinitions;
}

void FModificationUBTEditorToolPluginWizardDefinition::OnTemplateSelectionChanged(TSharedPtr<FPluginTemplateDescription> InSelectedItem, ESelectInfo::Type SelectInfo)
{
    if (InSelectedItem.IsValid() == false)
    {
        return;
    }

    if (
        const TSharedRef<FMyPluginTemplateDescription>* Found = this->PluginTemplateDefinitions.FindByPredicate(
        [InSelectedItem] (const TSharedRef<FMyPluginTemplateDescription>& Template)
        {
            return Template->TemplateDescription == InSelectedItem;
        })
    )
    {
        this->CurrentTemplateDefinition = *Found;
    }

    return;
}

bool FModificationUBTEditorToolPluginWizardDefinition::HasValidTemplateSelection(void) const
{
    return this->CurrentTemplateDefinition.IsValid();
}

TSharedPtr<FPluginTemplateDescription> FModificationUBTEditorToolPluginWizardDefinition::GetSelectedTemplate(void) const
{
    if (this->CurrentTemplateDefinition.IsValid() == false)
    {
        return nullptr;
    }

    return this->CurrentTemplateDefinition->TemplateDescription;
}

void FModificationUBTEditorToolPluginWizardDefinition::ClearTemplateSelection(void)
{
    this->CurrentTemplateDefinition.Reset();
}

bool FModificationUBTEditorToolPluginWizardDefinition::CanShowOnStartup() const
{
    return false;
}

bool FModificationUBTEditorToolPluginWizardDefinition::HasModules(void) const
{
    const FString SourceFolderPath = GetPluginFolderPath() / TEXT("Source");
    return FPaths::DirectoryExists(SourceFolderPath);
}

bool FModificationUBTEditorToolPluginWizardDefinition::IsMod(void) const
{
    return true;
}

void FModificationUBTEditorToolPluginWizardDefinition::OnShowOnStartupCheckboxChanged(ECheckBoxState CheckBoxState)
{
    return;
}

ECheckBoxState FModificationUBTEditorToolPluginWizardDefinition::GetShowOnStartupCheckBoxState(void) const
{
    return ECheckBoxState::Undetermined;
}

TSharedPtr<SWidget> FModificationUBTEditorToolPluginWizardDefinition::GetCustomHeaderWidget(void)
{
    return nullptr;
}

FText FModificationUBTEditorToolPluginWizardDefinition::GetInstructions(void) const
{
    return FText::FromString(TEXT("Choose template and specifiy a name for the new game plugin."));
}

bool FModificationUBTEditorToolPluginWizardDefinition::GetPluginIconPath(FString& OutIconPath) const
{
    if (this->CurrentTemplateDefinition.IsValid() == false)
    {
        return false;
    }

    return GetTemplateIconPath(this->CurrentTemplateDefinition.ToSharedRef()->TemplateDescription, OutIconPath);
}

EHostType::Type FModificationUBTEditorToolPluginWizardDefinition::GetPluginModuleDescriptor(void) const
{
    EHostType::Type ModuleDescriptorType = EHostType::Runtime;

    if (this->CurrentTemplateDefinition.IsValid())
    {
        ModuleDescriptorType = this->CurrentTemplateDefinition->TemplateDescription->ModuleDescriptorType;
    }

    return ModuleDescriptorType;
}

ELoadingPhase::Type FModificationUBTEditorToolPluginWizardDefinition::GetPluginLoadingPhase(void) const
{
    ELoadingPhase::Type Phase = ELoadingPhase::Default;

    if (this->CurrentTemplateDefinition.IsValid())
    {
        Phase = this->CurrentTemplateDefinition->TemplateDescription->LoadingPhase;
    }

    return Phase;
}

bool FModificationUBTEditorToolPluginWizardDefinition::GetTemplateIconPath(const TSharedRef<FPluginTemplateDescription> InTemplate, FString& OutIconPath) const
{
    bool bRequiresDefaultIcon = false;


    OutIconPath = this->GetFolderForTemplate(InTemplate) / TEXT("Resources/Icon128.png");
    if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*OutIconPath) == false)
    {
        OutIconPath = PluginBaseDir / TEXT("Resources/DefaultIcon128.png");
        bRequiresDefaultIcon = true;
    }

    return bRequiresDefaultIcon;
}

FString FModificationUBTEditorToolPluginWizardDefinition::GetPluginFolderPath(void) const
{
    return GetFolderForTemplate(this->CurrentTemplateDefinition.ToSharedRef()->TemplateDescription);
}

TArray<FString> FModificationUBTEditorToolPluginWizardDefinition::GetFoldersForSelection(void) const
{
    TArray<FString> SelectedFolders;

    if (this->CurrentTemplateDefinition.IsValid())
    {
        SelectedFolders.Add(GetFolderForTemplate(this->CurrentTemplateDefinition.ToSharedRef()->TemplateDescription));
    }

    return SelectedFolders;
}

void FModificationUBTEditorToolPluginWizardDefinition::PluginCreated(const FString& PluginName, const bool bWasSuccessful) const
{
    if (bWasSuccessful == false)
    {
        LOG_ERROR(LogModificationUBTEditorTool, "Plugin creation failed for %s.", *PluginName)
        return;
    }

    const TSharedPtr<IPlugin> NewPlugin = IPluginManager::Get().FindPlugin(PluginName);
    if (NewPlugin.IsValid() == false)
    {
        LOG_ERROR(LogModificationUBTEditorTool, "Plugin %s not found after creation.", *PluginName)
        return;
    }

    FPluginDescriptor Descriptor = NewPlugin->GetDescriptor();

    Descriptor.Version     = 1;
    Descriptor.VersionName = TEXT("0.0.1");
    Descriptor.AdditionalFieldsToWrite.Add(
        TEXT("JAFGGameVersion"),
        MakeShared<FJsonValueString>(FModificationUBTEditorToolModule::Get().GetCurrentGameVersion())
    );
    Descriptor.EngineVersion = FModificationUBTEditorToolModule::Get().GetEngineVersion();

    Descriptor.AdditionalFieldsToWrite.Add(TEXT("bIsClientOnly"), MakeShared<FJsonValueBoolean>(false));
    Descriptor.AdditionalFieldsToWrite.Add(TEXT("bIsServerOnly"), MakeShared<FJsonValueBoolean>(false));
    Descriptor.AdditionalFieldsToWrite.Add(TEXT("RemoteVersionRange"), MakeShared<FJsonValueString>(TEXT("0.0.1")));

    if (Descriptor.FriendlyName.IsEmpty()) { Descriptor.FriendlyName = PluginName; }
    if (Descriptor.Description.IsEmpty())  { Descriptor.Description  = TEXT("Game plugin for JAFG."); }
    Descriptor.Category = DEFAULT_JAFG_GAME_PLUGIN_CATEGORY;
    if (Descriptor.CreatedBy.IsEmpty())    { Descriptor.CreatedBy    = TEXT("Anonymous"); }

    Descriptor.AdditionalFieldsToWrite.Add(TEXT("bIsOptional"), MakeShared<FJsonValueBoolean>(false));

    {
#define ADD_PLUGIN_DEPENDENCY(PluginNameToAdd)                                                     \
{                                                                                                  \
    if (                                                                                           \
        const TSharedPtr<IPlugin> PluginToAdd = IPluginManager::Get().FindPlugin(PluginNameToAdd); \
        PluginToAdd.IsValid() == false                                                             \
    )                                                                                              \
    {                                                                                              \
        LOG_ERROR(LogModificationUBTEditorTool, "%s plugin not found.", *PluginNameToAdd)          \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        FPluginReferenceDescriptor Dependency(PluginNameToAdd, true);                              \
        Dependency.AdditionalFieldsToWrite.Add(                                                    \
            TEXT("VersionRange"),                                                                  \
            MakeShared<FJsonValueString>(                                                          \
                FModificationUBTEditorToolModule::Get().GetVersionOfCorePlugin(PluginNameToAdd)    \
            )                                                                                      \
        );                                                                                         \
        Descriptor.Plugins.Add(Dependency);                                                        \
    }                                                                                              \
}

        ADD_PLUGIN_DEPENDENCY(TEXT("CommonJAFGSlate"))
        ADD_PLUGIN_DEPENDENCY(TEXT("CommonSettings"))
        ADD_PLUGIN_DEPENDENCY(TEXT("JAFGExternalCore"))
        ADD_PLUGIN_DEPENDENCY(TEXT("JAFGGlobalLogging"))
        ADD_PLUGIN_DEPENDENCY(TEXT("ModificationSupervisor"))

#undef ADD_PLUGIN_DEPENDENCY
    }

    if (FText FailReason; NewPlugin->UpdateDescriptor(Descriptor, FailReason) == false)
    {
        LOG_ERROR(LogModificationUBTEditorTool, "Failed to update descriptor for plugin %s: %s.", *PluginName, *FailReason.ToString())
        return;
    }

    LOG_DISPLAY(LogModificationUBTEditorTool, "Plugin %s created successfully.", *PluginName)

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
FString FModificationUBTEditorToolPluginWizardDefinition::GetFolderForTemplate(const TSharedRef<FPluginTemplateDescription>& InTemplate) const
{
    return InTemplate->OnDiskPath;
}

TSharedPtr<FMyPluginTemplateDependency> FMyPluginTemplateDependency::Load(const TSharedPtr<FJsonObject>& InJSON, FString& OutError)
{
    FString DependencyName;
    if (InJSON->TryGetStringField(TEXT("name"), DependencyName) == false)
    {
        OutError = TEXT("\"name\" not in dependency");
        return nullptr;
    }

    FString DependencyVersion;
    if (InJSON->TryGetStringField(TEXT("version"), DependencyVersion) == false)
    {
        const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(DependencyName);
        if (Plugin.IsValid() == false)
        {
            OutError = TEXT("\"version\" not in dependency and plugin not installed");
            return nullptr;
        }

        DependencyVersion = Plugin->GetDescriptor().VersionName;
    }

    bool bOptional = false;
    InJSON->TryGetBoolField(TEXT("optional"), bOptional);

    return MakeShareable(new FMyPluginTemplateDependency(DependencyName, DependencyVersion, bOptional));
}

TSharedPtr<FMyPluginTemplateDescription> FMyPluginTemplateDescription::Load(const TSharedPtr<FJsonObject>& InJSON, const FString& InTemplatesPath, FString& OutError)
{
    FString TemplateName;
    if (InJSON->TryGetStringField(TEXT("name"), TemplateName) == false)
    {
        OutError = TEXT("\"name\" not in template");
        return nullptr;
    }

    FString TemplateDescription;
    if (InJSON->TryGetStringField(TEXT("description"), TemplateDescription) == false)
    {
        OutError = TEXT("\"description\" not in template");
        return nullptr;
    }

    FString TemplateFolderName;
    if(InJSON->TryGetStringField(TEXT("path"), TemplateFolderName) == false)
    {
        OutError = TEXT("\"path\" not in template");
        return nullptr;
    }

    TArray<TSharedPtr<FMyPluginTemplateDependency>> Dependencies;
    if (
        const TArray<TSharedPtr<FJsonValue>>* DependenciesPtr;
        InJSON->TryGetArrayField(TEXT("dependencies"), DependenciesPtr)
    )
    {
        int i = 0;
        for (const TSharedPtr<FJsonValue>& Item : *DependenciesPtr)
        {
            ++i;

            const TSharedPtr<FJsonObject>* DependencyObjPtr;
            if (Item->TryGetObject(DependencyObjPtr) == false)
            {
                OutError = FString::Printf(TEXT("dependency %d: not an object"), i);
                return nullptr;
            }

            FString DependencyError;
            TSharedPtr<FMyPluginTemplateDependency> Dependency =
                FMyPluginTemplateDependency::Load(*DependencyObjPtr, DependencyError);
            if (Dependency.IsValid() == false)
            {
                OutError = FString::Printf(TEXT("dependency %d: %s"), i, *DependencyError);
                return nullptr;
            }

            Dependencies.Add(Dependency);
        }
    }

    const TSharedRef<FPluginTemplateDescription> PluginTemplateDescription =
        MakeShareable(new FPluginTemplateDescription(
            FText::FromString(TemplateName),
            FText::FromString(TemplateDescription),
            InTemplatesPath / TemplateFolderName,
            true,
            EHostType::Runtime
        ));
    PluginTemplateDescription->bCanBePlacedInEngine = false;
    return MakeShareable(new FMyPluginTemplateDescription(PluginTemplateDescription, Dependencies));
}
