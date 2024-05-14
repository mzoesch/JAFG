﻿// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FJAFGCoreModule final : public IModuleInterface
{

public:

    // IModuleInterface interface
    virtual void StartupModule(void) override;
    virtual void ShutdownModule(void) override;
    // ~IModuleInterface interface
};
