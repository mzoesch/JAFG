// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModificationUBTEditorToolPluginProfile.h"
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

    auto IsPluginConsideredAsModification(const IPlugin& Plugin) const -> bool;

    static const FName DevelopmentTabName;
    static const FName ShippingTabName;
    static const FName PluginCreatorTabName;
    static const FName LogTabName;

    auto PackagePluginsDevelopment(TArray<TSharedRef<IPlugin>> Plugins) -> void;
    auto PackagePluginsShipping(TArray<TSharedRef<IPlugin>> Plugins) -> void;
    auto PackagePlugins(TArray<TSharedRef<FModificationUBTEditorToolPluginProfile>> Plugins) -> void;

    FORCEINLINE auto IsPackaging(void) const -> bool { return this->bIsPackaging; }

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
    /*
     * This might be weird as this is not a queue but an array. We have to use an array, as we cannot dequeue
     * a shared reference from a queue. We would need firstly to declare a shared reference with an empty pointer
     * / constructor, which is not allowed and only possible deep in the engine code inside the hot reload subsystem.
     *
     * bool Dequeue(T& Out); <-- This is not possible with shared references.
     *
     * We could, of course, implement our own queue (as we did in the primary game module) to implement our own
     * (non-standardized) effects, but that would also defeat the purpose of using a queue like structure in
     * the first place.
     */
    TArray<TSharedRef<FModificationUBTEditorToolPluginProfile>> ProfilePackageQueue;
    void ProcessQueueItem(void);

    void RegisterPluginTemplates(void);
    void AddPluginTemplatesFromPlugin(const IPlugin& Plugin);
    TArray<TSharedRef<FMyPluginTemplateDescription>> PluginTemplateDefinitions;
};
