// Copyright 2024 mzoesch. All rights reserved.

#include "ExternalModificationSubsystem.h"

#include "ModificationSupervisorSubsystem.h"

void UExternalModificationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UModificationSupervisorSubsystem>();
    Super::Initialize(Collection);

    UModificationSupervisorSubsystem* ModSupervisor = this->GetGameInstance()->GetSubsystem<UModificationSupervisorSubsystem>();
    jcheck( ModSupervisor )

    ModSupervisor->InitializeOptionalVoxelsDelegate.AddLambda(
    [this] (TArray<FVoxelMask>& VoxelMasks)
    {
        this->OnOptionalVoxelsInitialize(VoxelMasks);
    });

    ModSupervisor->InitializeOptionalItemsDelegate.AddLambda(
    [this] (TArray<FItemMask>& ItemMasks)
    {
        this->OnOptionalItemsInitialize(ItemMasks);
    });

    return;
}
