// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "RegistrySubsystem.generated.h"

class USettingRegistry;
JAFG_VOID

UCLASS()
class JAFGCORE_API URegistrySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    // Subsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~Subsystem implementation

public:

    TObjectPtr<USettingRegistry> SettingRegistry;
};
