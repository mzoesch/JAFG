// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "Modules/ModuleInterface.h"

class MODIFICATIONSUPERVISOR_API IJAFGModuleInterface : public IModuleInterface
{

public:

    // IModuleInterface implementation
    virtual void StartupModule(void) override;
    virtual void ShutdownModule(void) override;
    // ~IModuleInterface implementation
};
