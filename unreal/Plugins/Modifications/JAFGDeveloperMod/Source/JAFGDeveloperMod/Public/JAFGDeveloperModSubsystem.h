// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExternalModificationSubsystem.h"
#include "JAFGDeveloperModSubsystem.generated.h"

UCLASS(NotBlueprintable)
class JAFGDEVELOPERMOD_API UJAFGDeveloperModSubsystem : public UExternalModificationSubsystem
{
    GENERATED_BODY()

    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // ~USubsystem implementation

protected:

    // UExternalModificationSubsystem implementation
    virtual void OnOptionalVoxelsInitialize(TArray<FVoxelMask>& OutVoxelMasks) override;
    virtual void OnOptionalItemsInitialize(TArray<FItemMask>& OutItemMasks) override;
    // ~UExternalModificationSubsystem implementation
};
