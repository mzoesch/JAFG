// © 2023 mzoesch. All rights reserved.

#include "Core/GI_Master.h"

#include "World/Voxel.h"
#include "World/WorldVoxel.h"
#include "World/VoxelMask.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

void UGI_Master::Init()
{
    Super::Init();

    this->InitializeVoxels();
    UIL_LOG(Warning, TEXT("Initialized %d voxel masks."), this->VoxelMasks.Num());

    return;
}

void UGI_Master::InitializeVoxels()
{
    this->VoxelMasks.Add(FVoxelMask("CORE", "NullVoxel", true));
    this->VoxelMasks.Add(FVoxelMask("CORE", "AirVoxel", true));

    this->VoxelMasks.Add(FVoxelMask("JAFG", "StoneVoxel", false));
    this->VoxelMasks.Add(FVoxelMask("JAFG", "DirtVoxel", false));
    this->VoxelMasks.Add(FVoxelMask("JAFG", "GrassVoxel", false));
    
    return;
}
