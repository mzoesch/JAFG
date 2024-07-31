// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGDeveloperModSubsystem.h"
#include "JAFGLogDefs.h"

void UJAFGDeveloperModSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_DISPLAY(LogModSubsystem, "Called.")

    return;
}

void UJAFGDeveloperModSubsystem::OnOptionalVoxelsInitialize(TArray<FVoxelMask>& OutVoxelMasks)
{
    Super::OnOptionalVoxelsInitialize(OutVoxelMasks);

    LOG_DISPLAY(LogModSubsystem, "JAFG Developer mod is initializing its voxels.")

    return;
}

void UJAFGDeveloperModSubsystem::OnOptionalItemsInitialize(TArray<FItemMask>& OutItemMasks)
{
    Super::OnOptionalItemsInitialize(OutItemMasks);

    LOG_DISPLAY(LogModSubsystem, "JAFG Developer mod is initializing its items.")

    return;
}
