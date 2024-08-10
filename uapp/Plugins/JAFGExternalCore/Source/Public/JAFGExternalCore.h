// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#define JAFG_EXTERNAL_CORE_MODULE_NAME "JAFGExternalCore"

DECLARE_DELEGATE_OneParam(FOnInitInternalSubsystem, FSubsystemCollectionBase& /* Collection */)

class FJAFGExternalCoreModule final : public IModuleInterface
{

public:

    // IModuleInterface implementation
    virtual void ShutdownModule(void) override;
    virtual void StartupModule(void) override;
    // ~IModuleInterface implementation

    FORCEINLINE static auto Get(void) -> FJAFGExternalCoreModule&
    {
        return FModuleManager::GetModuleChecked<FJAFGExternalCoreModule>(JAFG_EXTERNAL_CORE_MODULE_NAME);
    }

    /**
     * The game itself, not any plugin, has a subsystem initialized prior all other subsystem.
     */
    FOnInitInternalSubsystem OnInitInternalSubsystem;
#if !UE_BUILD_SHIPPING
    bool bInternalSubsystemInitialized = false;
#endif /* !UE_BUILD_SHIPPING */
};
