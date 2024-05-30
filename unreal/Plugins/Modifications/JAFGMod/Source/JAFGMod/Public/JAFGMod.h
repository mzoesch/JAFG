// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FJAFGModModule final : public IModuleInterface
{

public:

    // IModuleInterface implementation
    virtual void StartupModule(void) override;
    virtual void ShutdownModule(void) override;
    // ~IModuleInterface implementation
};
