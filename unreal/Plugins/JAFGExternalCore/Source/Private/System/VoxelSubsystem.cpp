// Copyright 2024 mzoesch. All rights reserved.

#include "System/VoxelSubsystem.h"
#include "Accumulated.h"
#include "System/PreInternalInitializationSubsystemRequirements.h"

UVoxelSubsystem::UVoxelSubsystem(void) : Super()
{
    return;
}

void UVoxelSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_VERBOSE(LogVoxelSubsystem, "Called.")

    this->InitializeCommonVoxels();
    this->InitializeOptionalVoxels();

    GAccumulatedItemIndexStart = this->GetItemIndexStart();
    jcheck( GAccumulatedItemIndexStart > 0 )

    LOG_DISPLAY(LogVoxelSubsystem, "Voxel Subsystem initialized with [%d/%d] voxels.", this->CommonVoxelNum, this->VoxelMasks.Num() )

    return;
}

void UVoxelSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

auto UVoxelSubsystem::GetVoxelIndex(const FString& NameSpace, const FString& Name) const -> uint32
{
    for (voxel_t_signed i = 0; i < this->VoxelMasks.Num(); ++i)
    {
        if (this->VoxelMasks[i].NameSpace == NameSpace && this->VoxelMasks[i].Name == Name)
        {
            return i;
        }
    }

    return ECommonVoxels::Null;
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

    jcheck( this->CommonVoxelNum != -1 && TEXT("Common voxels not initialized.") )

    return;
}

void UVoxelSubsystem::InitializeOptionalVoxels(void)
{
    TArray<UGameInstanceSubsystem*> Arr =
        this->GetGameInstance()->GetSubsystemArray<UGameInstanceSubsystem>();

    for (UGameInstanceSubsystem* Subsystem : Arr)
    {
        IPreInternalInitializationSubsystemRequirements* SubsystemInterface =
            Cast<IPreInternalInitializationSubsystemRequirements>(Subsystem);

        if (SubsystemInterface)
        {
            SubsystemInterface->InitializeOptionalVoxels(this->VoxelMasks);
            return;
        }

    }

    LOG_FATAL(LogVoxelSubsystem, "No subsystem found that implements the IPreInternalInitializationSubsystemRequirements interface.")

    return;
}
