// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPluginWizardDefinition.h"

struct FMyPluginTemplateDescription;

class FModificationUBTEditorToolPluginWizardDefinition final : public IPluginWizardDefinition
{
public:

    FModificationUBTEditorToolPluginWizardDefinition(void);
    virtual ~FModificationUBTEditorToolPluginWizardDefinition(void) override = default;

    // IPluginWizardDefinition interface
    virtual auto GetTemplatesSource(void) const -> const TArray<TSharedRef<FPluginTemplateDescription>>& override;
    virtual auto OnTemplateSelectionChanged(TSharedPtr<FPluginTemplateDescription> InSelectedItem, ESelectInfo::Type SelectInfo) -> void override;
    virtual auto HasValidTemplateSelection(void) const -> bool override;
    virtual auto GetSelectedTemplate(void) const -> TSharedPtr<FPluginTemplateDescription> override;
    virtual auto ClearTemplateSelection(void) -> void override;

    virtual auto CanShowOnStartup(void) const -> bool override;
    virtual auto HasModules(void) const -> bool override;
    virtual auto IsMod(void) const -> bool override;
    virtual auto OnShowOnStartupCheckboxChanged(const ECheckBoxState CheckBoxState) -> void override;
    virtual auto GetShowOnStartupCheckBoxState(void) const -> ECheckBoxState override;
    virtual auto GetCustomHeaderWidget(void) -> TSharedPtr<class SWidget> override;
    virtual auto GetInstructions(void) const -> FText override;

    virtual auto GetPluginIconPath(FString& OutIconPath) const -> bool override;
    virtual auto GetPluginModuleDescriptor(void) const -> EHostType::Type override;
    virtual auto GetPluginLoadingPhase(void) const -> ELoadingPhase::Type override;
    virtual auto GetTemplateIconPath(const TSharedRef<FPluginTemplateDescription> InTemplate, FString& OutIconPath) const -> bool override;
    virtual auto GetPluginFolderPath(void) const -> FString override;
    virtual auto GetFoldersForSelection(void) const -> TArray<FString> override;
    virtual auto PluginCreated(const FString& PluginName, const bool bWasSuccessful) const -> void override;
    // ~IPluginWizardDefinition interface

private:

    /** Gets the folder for the specified template. */
    FString GetFolderForTemplate(const TSharedRef<FPluginTemplateDescription>& InTemplate) const;

    /** The templates available to this definition. */
    TArray<TSharedRef<FMyPluginTemplateDescription>> PluginTemplateDefinitions;

    /** The inner templates available to this definition. */
    TArray<TSharedRef<FPluginTemplateDescription>> TemplateDefinitions;

    /** The currently selected template definition. */
    TSharedPtr<FMyPluginTemplateDescription> CurrentTemplateDefinition;

    /** Base directory of the plugin templates. */
    FString PluginBaseDir;
};

struct FMyPluginTemplateDependency
{
    FString Name;
    FString Version;
    bool bOptional;
    bool bBasePlugin;

    explicit FMyPluginTemplateDependency(const FString& InName, const FString& InVersion, const bool bInOptional, const bool bInBasePlugin)
        : Name(InName)
        , Version(InVersion)
        , bOptional(bInOptional)
        , bBasePlugin(bInBasePlugin)
    {
        return;
    }

    static TSharedPtr<FMyPluginTemplateDependency> Load(const TSharedPtr<FJsonObject>& InJSON, FString& OutError);
};


struct FMyPluginTemplateDescription
{
    TSharedRef<FPluginTemplateDescription> TemplateDescription;

    TArray<TSharedPtr<FMyPluginTemplateDependency>> Dependencies;

    FMyPluginTemplateDescription(
        const TSharedRef<FPluginTemplateDescription>& InTemplateDescription,
        const TArray<TSharedPtr<FMyPluginTemplateDependency>>& InDependencies
    )
        : TemplateDescription(InTemplateDescription)
        , Dependencies(InDependencies)
    {
        return;
    }

    static auto Load(
        const TSharedPtr<FJsonObject>& InJSON,
        const FString& InTemplatesPath, FString& OutError
    ) -> TSharedPtr<FMyPluginTemplateDescription>;
};
