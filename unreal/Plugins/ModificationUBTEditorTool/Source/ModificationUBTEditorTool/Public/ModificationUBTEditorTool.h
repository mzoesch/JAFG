// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

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

    static const FName DevelopmentTabName;
    static const FName ShippingTabName;

    void PackagePluginsDevelopment(TArray<TSharedRef<IPlugin>> Plugins);
    void PackagePluginsShipping(TArray<TSharedRef<IPlugin>> Plugins);

    FORCEINLINE bool IsPackaging(void) const { return this->bIsPackaging; }

private:

    void RegisterSettings(void) const;
    void UnregisterSettings(void) const;

    TSharedPtr<FUICommandList> PluginCommands;

    /* No need to make this thread safe, as it can only be set to false by the packaging thread. */
    bool bIsPackaging = false;
};
