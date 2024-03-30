// Copyright 2024 mzoesch. All rights reserved.


#include "World/Voxel/VoxelSubsystem.h"

void UVoxelSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("UVoxelSubsystem::Initialize: Initializing Voxel Subsystem."))

	this->InitializeCommonVoxels();
	this->InitializeOptionalVoxels();

	UE_LOG(LogTemp, Log, TEXT("UVoxelSubsystem::Initialize: Voxel Subsystem initialized with [%d/%d]."), this->GetCommonVoxelNum(), this->VoxelMasks.Num())

	return;
}

void UVoxelSubsystem::Deinitialize(void)
{
	Super::Deinitialize();
}

void UVoxelSubsystem::SetCommonVoxelNum(void)
{
	/* If only common voxels are currently initialize we need this. */
	int32 FallbackRet = -1;
	for (int32 i = 0; i < this->VoxelMasks.Num(); i++)
	{
		FallbackRet = ++i;
		
		if (this->VoxelMasks[i].NameSpace == TEXT("COMMON"))
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

	if (this->CommonVoxelNum == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("UVoxelSubsystem::InitializeCommonVoxels: No common voxels failed to initialize."))
		return;
	}
	
	return;
}

void UVoxelSubsystem::InitializeOptionalVoxels(void)
{
	this->VoxelMasks.Add(FVoxelMask(TEXT("JAFG"), TEXT("Stone")));
	this->VoxelMasks.Add(FVoxelMask(TEXT("JAFG"), TEXT("Dirt")));
	this->VoxelMasks.Add(FVoxelMask(TEXT("JAFG"), TEXT("Grass")));
}
