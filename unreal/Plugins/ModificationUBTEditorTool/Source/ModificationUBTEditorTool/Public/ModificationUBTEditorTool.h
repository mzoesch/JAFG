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

    static const FName DevelopmentTabName;
    static const FName ShippingTabName;

private:

    void RegisterSettings(void) const;
    void UnregisterSettings(void) const;

    TSharedPtr<FUICommandList> PluginCommands;
};
