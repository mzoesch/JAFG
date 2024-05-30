// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "JAFGGameInstanceSubsystem.generated.h"

/**
 * Makes sure to initialize mods first, so that they can hook into some game instance subsystems and change their
 * initialization behavior.
 */
UCLASS(Abstract)
class JAFG_API UJAFGGameInstanceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    // Subsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // ~Subsystem implementation

    template<class T>
    T* GetTemplateGameInstance(void) const
    {
        return Cast<T>(Super::GetGameInstance());
    }
};
