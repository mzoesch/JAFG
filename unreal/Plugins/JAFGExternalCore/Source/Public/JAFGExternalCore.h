// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FJAFGExternalCoreModule final : public IModuleInterface
{

public:

    // IModuleInterface implementation
    virtual void ShutdownModule(void) override;
    virtual void StartupModule(void) override;
    // ~IModuleInterface implementation
};
