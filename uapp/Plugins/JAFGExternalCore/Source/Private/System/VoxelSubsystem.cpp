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

voxel_t UVoxelSubsystem::GetSafeVoxelIndex(const FString& Name) const
{
    if (const voxel_t Voxel = this->GetVoxelIndex(Name); Voxel != ECommonVoxels::Null)
    {
        return Voxel;
    }

    LOG_FATAL(LogVoxelSubsystem, "Voxel [%s] not found.", *Name)

    return ECommonVoxels::Null;
}

voxel_t UVoxelSubsystem::GetVoxelIndex(const FString& NameSpace, const FString& Name) const
{
    for (voxel_t_signed i = 0; i < this->VoxelMasks.Num(); ++i)
    {
        if (this->VoxelMasks[i].Namespace == NameSpace && this->VoxelMasks[i].Name == Name)
        {
            return i;
        }
    }

    return ECommonVoxels::Null;
}

voxel_t UVoxelSubsystem::GetSafeVoxelIndex(const FString& Namespace, const FString& Name) const
{
    if (const voxel_t Voxel = this->GetVoxelIndex(Namespace, Name); Voxel != ECommonVoxels::Null)
    {
        return Voxel;
    }

    LOG_FATAL(LogVoxelSubsystem, "Voxel [%s] not found.", *Name)

    return ECommonVoxels::Null;
}

voxel_t UVoxelSubsystem::GetItemIndex(const FString& Name) const
{
    for (voxel_t_signed i = 0; i < this->ItemMasks.Num(); ++i)
    {
        if (this->ItemMasks[i].Name == Name)
        {
            return this->TransformItemToAccumulated(i);
        }
    }

    return ECommonAccumulated::Null;
}

voxel_t UVoxelSubsystem::GetItemIndex(const FString& Namespace, const FString& Name) const
{
    for (voxel_t_signed i = 0; i < this->ItemMasks.Num(); ++i)
    {
        if (this->ItemMasks[i].Namespace == Namespace && this->ItemMasks[i].Name == Name)
        {
            return this->TransformItemToAccumulated(i);
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

voxel_t UVoxelSubsystem::GetSafeAccumulatedIndex(const FString& Name) const
{
    if (const voxel_t Accumulated = this->GetAccumulatedIndex(Name); Accumulated != ECommonAccumulated::Null)
    {
        return Accumulated;
    }

    LOG_FATAL(LogVoxelSubsystem, "Accumulated [%s] not found.", *Name)

    return ECommonAccumulated::Null;
}

voxel_t UVoxelSubsystem::GetAccumulatedIndex(const FString& NameSpace, const FString& Name) const
{
    if (const voxel_t Voxel = this->GetVoxelIndex(NameSpace, Name); Voxel != ECommonAccumulated::Null)
    {
        return Voxel;
    }

    return this->GetItemIndex(NameSpace, Name);
}

voxel_t UVoxelSubsystem::GetSafeAccumulatedIndex(const FString& Namespace, const FString& Name) const
{
    if (const voxel_t Accumulated = this->GetAccumulatedIndex(Namespace, Name); Accumulated != ECommonAccumulated::Null)
    {
        return Accumulated;
    }

    LOG_FATAL(LogVoxelSubsystem, "Accumulated [%s] not found.", *Name)

    return ECommonAccumulated::Null;
}

bool UVoxelSubsystem::GetMaybeUndefinedVoxel(const voxel_t Voxel, FString& OutName, FString& OutNamespace) const
{
    if (this->VoxelMasks.IsValidIndex(Voxel))
    {
        OutName      = this->VoxelMasks[Voxel].Name;
        OutNamespace = this->VoxelMasks[Voxel].Namespace;
        return true;
    }

    return false;
}

bool UVoxelSubsystem::GetMaybeUndefinedVoxelName(const voxel_t Voxel, FString& OutName) const
{
    FString Unused;
    return this->GetMaybeUndefinedVoxel(Voxel, OutName, Unused);
}

bool UVoxelSubsystem::GetMaybeUndefinedVoxelNamespace(const voxel_t Voxel, FString& OutNamespace) const
{
    FString Unused;
    return this->GetMaybeUndefinedVoxel(Voxel, Unused, OutNamespace);
}

bool UVoxelSubsystem::GetMaybeUndefinedItem(const voxel_t Item, FString& OutName, FString& OutNamespace) const
{
    if (this->ItemMasks.IsValidIndex(this->TransformAccumulatedToItem(Item)))
    {
        OutName      = this->ItemMasks[this->TransformAccumulatedToItem(Item)].Name;
        OutNamespace = this->ItemMasks[this->TransformAccumulatedToItem(Item)].Namespace;
        return true;
    }

    return false;
}

bool UVoxelSubsystem::GetMaybeUndefinedItemName(const voxel_t Item, FString& OutName) const
{
    FString Unused;
    return this->GetMaybeUndefinedItem(Item, OutName, Unused);
}

bool UVoxelSubsystem::GetMaybeUndefinedItemNamespace(const voxel_t Item, FString& OutNamespace) const
{
    FString Unused;
    return this->GetMaybeUndefinedItem(Item, Unused, OutNamespace);
}

bool UVoxelSubsystem::GetMaybeUndefinedAccumulated(const voxel_t Accumulated, FString& OutName, FString& OutNamespace) const
{
    if (this->GetMaybeUndefinedVoxel(Accumulated, OutName, OutNamespace))
    {
        return true;
    }

    return this->GetMaybeUndefinedItem(Accumulated, OutName, OutNamespace);
}

bool UVoxelSubsystem::GetMaybeUndefinedAccumulatedName(const voxel_t Accumulated, FString& OutName) const
{
    FString Unused;
    return this->GetMaybeUndefinedAccumulated(Accumulated, OutName, Unused);
}

bool UVoxelSubsystem::GetMaybeUndefinedAccumulatedNamespace(const voxel_t Accumulated, FString& OutNamespace) const
{
    FString Unused;
    return this->GetMaybeUndefinedAccumulated(Accumulated, Unused, OutNamespace);
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

voxel_t UVoxelSubsystem::GetItemRealIndex(const FString& Name) const
{
    for (voxel_t_signed i = 0; i < this->ItemMasks.Num(); ++i)
    {
        if (this->ItemMasks[i].Name == Name)
        {
            return i;
        }
    }

    LOG_WARNING(LogVoxelSubsystem, "Item [?::%s] not found.", *Name)

    /*
     * The first index. Note that ECommonAccumulated::Null does not make sense in this context the first index is
     * in fact not null but the first valid item.
     */
    return 0;
}

voxel_t UVoxelSubsystem::GetItemRealIndex(const FString& Namespace, const FString& Name) const
{
    for (voxel_t_signed i = 0; i < this->ItemMasks.Num(); ++i)
    {
        if (this->ItemMasks[i].Namespace == Namespace && this->ItemMasks[i].Name == Name)
        {
            return i;
        }
    }

    LOG_WARNING(LogVoxelSubsystem, "Item [%s::%s] not found.", *Namespace, *Name)

    /*
     * The first index. Note that ECommonAccumulated::Null does not make sense in this context the first index is
     * in fact not null but the first valid item.
     */
    return 0;
}
