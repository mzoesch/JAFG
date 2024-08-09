// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VoxelMask.h"
#include "ItemMask.h"

#include "ExternalModificationSubsystem.generated.h"

UCLASS(Abstract)
class MODIFICATIONSUPERVISOR_API UExternalModificationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // ~USubsystem implementation

protected:

    virtual void OnOptionalVoxelsInitialize(TArray<FVoxelMask>& OutVoxelMasks) { }
    virtual void OnOptionalItemsInitialize(TArray<FItemMask>& OutItemMasks) { }
};
