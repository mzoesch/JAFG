// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExternalModificationSubsystem.h"

#include "JAFGModSubsystem.generated.h"

struct FVoxelMask;
struct FItemMask;

UCLASS(NotBlueprintable)
class JAFGMOD_API UJAFGModSubsystem : public UExternalModificationSubsystem
{
    GENERATED_BODY()

public:

    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // ~USubsystem implementation

protected:

    void OnOptionalVoxelsInitialize(TArray<FVoxelMask>& VoxelMasks);
    void OnOptionalItemsInitialize(TArray<FItemMask>& ItemMasks);
};
