// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "System/PreInternalInitializationSubsystemRequirements.h"

#include "PreInternalInitializationSubsystem.generated.h"

struct FVoxelMask;

/**
 * Makes sure to initialize mods first, so that they can hook into some game instance subsystems and change their
 * initialization behavior.
 */
UCLASS(NotBlueprintable)
class JAFG_API UPreInternalInitializationSubsystem final : public UGameInstanceSubsystem, public IPreInternalInitializationSubsystemRequirements
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

    // IPreInternalInitializationSubsystemRequirements interface
    virtual void InitializeOptionalVoxels(TArray<FVoxelMask>& VoxelMasks) override;
    virtual void InitializeOptionalItems(TArray<FItemMask>& VoxelMasks) override;
    // ~IPreInternalInitializationSubsystemRequirements interface
};
