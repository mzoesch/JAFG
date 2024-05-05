// Copyright 2024 mzoesch. All rights reserved.

#include "System/VoxelSubsystem.h"

UVoxelSubsystem::UVoxelSubsystem(void) : Super()
{
    return;
}

void UVoxelSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_VERBOSE(LogVoxelSubsystem, "Called." )

    this->InitializeCommonVoxels();
    this->InitializeOptionalVoxels();

    Accumulated::Initialize(this->GetItemIndexStart());

    LOG_DISPLAY(LogVoxelSubsystem, "Voxel Subsystem initialized with [%d/%d] voxels.", this->CommonVoxelNum, this->VoxelMasks.Num() )

    return;
}

void UVoxelSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

void UVoxelSubsystem::SetCommonVoxelNum(void)
{
    /* If only common voxels are currently initialized, we need this. */
    int32 FallbackRet = -1;

    for (int32 i = 0; i < this->VoxelMasks.Num(); ++i)
    {
        FallbackRet = i + 1;

        if (this->VoxelMasks[i].NameSpace == CommonNamespace)
        {
            continue;
        }

        this->CommonVoxelNum = ++i;
        return;
    }

    this->CommonVoxelNum = FallbackRet;

    return;
}

void UVoxelSubsystem::InitializeCommonVoxels(void)
{
    this->VoxelMasks.Add(FVoxelMask::Null);
    this->VoxelMasks.Add(FVoxelMask::Air);

    this->SetCommonVoxelNum();

    check( this->CommonVoxelNum != -1 && TEXT("Common voxels not initialized.") )

    return;
}

void UVoxelSubsystem::InitializeOptionalVoxels(void)
{
    this->VoxelMasks.Add(FVoxelMask(JAFGNamespace, TEXT("Stone"), ETextureGroup::Opaque));
    this->VoxelMasks.Add(FVoxelMask(JAFGNamespace, TEXT("Dirt"), ETextureGroup::Opaque));
    this->VoxelMasks.Add(FVoxelMask(JAFGNamespace, TEXT("Grass"), ETextureGroup::Opaque));

    return;
}
