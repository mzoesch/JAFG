// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGModSubsystem.h"

#include "JAFGLogDefs.h"
#include "ModificationSupervisorSubsystem.h"
#include "JAFGModTypes.h"

void UJAFGModSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_DISPLAY(LogModSubsystem, "Called.")

    UModificationSupervisorSubsystem* ModSupervisor = this->GetGameInstance()->GetSubsystem<UModificationSupervisorSubsystem>();

    check( ModSupervisor )

    ModSupervisor->InitializeOptionalVoxelsEvent.AddLambda(
    [this] (TArray<FVoxelMask>& VoxelMasks)
    {
        this->OnOptionalVoxelsInitialize(VoxelMasks);
    });

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UJAFGModSubsystem::OnOptionalVoxelsInitialize(TArray<FVoxelMask>& VoxelMasks)
{
    LOG_DISPLAY(LogModSubsystem, "JAFG mod is initializing its voxels.")

    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("StoneVoxel")));
    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("DirtVoxel")));
    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("GrassVoxel")));
    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("OakPlanksVoxel")));
    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("OakLogVoxel")));
    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("CraftingTableVoxel")));

    return;
}
