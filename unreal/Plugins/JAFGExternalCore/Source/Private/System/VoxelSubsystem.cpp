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

    this->InitializeOptionalItems();

    LOG_DISPLAY(LogVoxelSubsystem, "Voxel Subsystem initialized with [%d] items.", this->ItemMasks.Num() )

    return;
}

void UVoxelSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

voxel_t UVoxelSubsystem::GetVoxelIndex(const FString& Name) const
{
    for (voxel_t_signed i = 0; i < this->VoxelMasks.Num(); ++i)
    {
        if (this->VoxelMasks[i].Name == Name)
        {
            return i;
        }
    }

    return ECommonVoxels::Null;
}

uint32 UVoxelSubsystem::GetVoxelIndex(const FString& NameSpace, const FString& Name) const
{
    for (voxel_t_signed i = 0; i < this->VoxelMasks.Num(); ++i)
    {
        if (this->VoxelMasks[i].Namespace == NameSpace && this->VoxelMasks[i].Name == Name)
        {
            return i;
        }
    }

    return ECommonAccumulated::Null;
}

voxel_t UVoxelSubsystem::GetItemIndex(const FString& Name) const
{
    for (voxel_t_signed i = 0; i < this->ItemMasks.Num(); ++i)
    {
        if (this->ItemMasks[i].Name == Name)
        {
            return i;
        }
    }

    return ECommonAccumulated::Null;
}

voxel_t UVoxelSubsystem::GetItemIndex(const FString& NameSpace, const FString& Name) const
{
    for (voxel_t_signed i = 0; i < this->ItemMasks.Num(); ++i)
    {
        if (this->ItemMasks[i].Namespace == NameSpace && this->ItemMasks[i].Name == Name)
        {
            return i;
        }
    }

    return ECommonAccumulated::Null;
}

voxel_t UVoxelSubsystem::GetAccumulatedIndex(const FString& Name) const
{
    if (const voxel_t Voxel = this->GetVoxelIndex(Name); Voxel != ECommonAccumulated::Null)
    {
        return Voxel;
    }

    return this->GetItemIndex(Name);
}

voxel_t UVoxelSubsystem::GetAccumulatedIndex(const FString& NameSpace, const FString& Name) const
{
    if (const voxel_t Voxel = this->GetVoxelIndex(NameSpace, Name); Voxel != ECommonAccumulated::Null)
    {
        return Voxel;
    }

    return this->GetItemIndex(NameSpace, Name);
}

void UVoxelSubsystem::SetCommonVoxelNum(void)
{
    /* If only common voxels are currently initialized, we need this. */
    int32 FallbackRet = -1;

    for (int32 i = 0; i < this->VoxelMasks.Num(); ++i)
    {
        FallbackRet = i + 1;

        if (this->VoxelMasks[i].Namespace == CommonNamespace)
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
    for (UGameInstanceSubsystem* Subsystem : this->GetGameInstance()->GetSubsystemArray<UGameInstanceSubsystem>())
    {
        if (
            IPreInternalInitializationSubsystemRequirements* SubsystemInterface =
                Cast<IPreInternalInitializationSubsystemRequirements>(Subsystem)
        )
        {
            SubsystemInterface->InitializeOptionalVoxels(this->VoxelMasks);
            return;
        }

        continue;
    }

    LOG_FATAL(LogVoxelSubsystem, "No subsystem found that implements the IPreInternalInitializationSubsystemRequirements interface.")

    return;
}

void UVoxelSubsystem::InitializeOptionalItems(void)
{
    for (UGameInstanceSubsystem* Subsystem : this->GetGameInstance()->GetSubsystemArray<UGameInstanceSubsystem>())
    {
        if (
            IPreInternalInitializationSubsystemRequirements* SubsystemInterface =
                Cast<IPreInternalInitializationSubsystemRequirements>(Subsystem)
        )
        {
            SubsystemInterface->InitializeOptionalItems(this->ItemMasks);
            return;
        }

        continue;
    }

    LOG_FATAL(LogVoxelSubsystem, "No subsystem found that implements the IPreInternalInitializationSubsystemRequirements interface.")

    return;
}
