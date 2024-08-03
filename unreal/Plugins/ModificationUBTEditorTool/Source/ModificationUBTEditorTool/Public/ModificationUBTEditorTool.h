// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModificationUBTEditorToolPluginWizardDefinition.h"
#include "Modules/ModuleManager.h"

#define DEFAULT_JAFG_GAME_PLUGIN_CATEGORY TEXT("JAFGGamePlugin")

class FModificationUBTEditorToolModule final : public IModuleInterface
{
public:

    // IModuleInterface interface
    virtual void StartupModule(void) override;
    virtual void ShutdownModule(void) override;
    // ~IModuleInterface interface

    FORCEINLINE static auto Get(void) -> FModificationUBTEditorToolModule&
    {
        return FModuleManager::GetModuleChecked<FModificationUBTEditorToolModule>("ModificationUBTEditorTool");
    }

    auto GetCurrentGameVersion(void) -> FString;
    auto GetEngineVersion(void) -> FString;
    auto GetVersionOfCorePlugin(const FString& CorePluginName) -> FString;

    bool IsPluginConsideredAsModification(const IPlugin& Plugin) const;

    static const FName DevelopmentTabName;
    static const FName ShippingTabName;
    static const FName PluginCreatorTabName;

    void PackagePluginsDevelopment(TArray<TSharedRef<IPlugin>> Plugins);
    void PackagePluginsShipping(TArray<TSharedRef<IPlugin>> Plugins);

    FORCEINLINE bool IsPackaging(void) const { return this->bIsPackaging; }

    FORCEINLINE auto GetPluginTemplates(void) -> TArray<TSharedRef<FMyPluginTemplateDescription>>&
    {
        return this->PluginTemplateDefinitions;
    }
    FORCEINLINE auto GetPluginTemplates(void) const -> const TArray<TSharedRef<FMyPluginTemplateDescription>>&
    {
        return this->PluginTemplateDefinitions;
    }

private:

    void RegisterSettings(void) const;
    void UnregisterSettings(void) const;

    TSharedPtr<FUICommandList> PluginCommands;

    /* No need to make this thread safe, as it can only be set to false by the packaging thread. */
    bool bIsPackaging = false;

    void RegisterPluginTemplates(void);
    void AddPluginTemplatesFromPlugin(const IPlugin& Plugin);
    TArray<TSharedRef<FMyPluginTemplateDescription>> PluginTemplateDefinitions;
};
