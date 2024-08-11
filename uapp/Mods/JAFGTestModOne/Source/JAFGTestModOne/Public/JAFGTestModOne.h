// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGModuleInterface.h"
#include "Modules/ModuleManager.h"

class FJAFGTestModOneModule final : public IJAFGModuleInterface
{

public:

    // IModuleInterface implementation
    virtual void StartupModule(void) override;
    virtual void ShutdownModule(void) override;
    // ~IModuleInterface implementation
};
