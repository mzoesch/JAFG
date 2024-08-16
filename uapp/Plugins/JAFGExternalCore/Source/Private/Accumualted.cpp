// Copyright 2024 mzoesch. All rights reserved.

#include "Accumulated.h"
#include "System/VoxelSubsystem.h"

voxel_t GAccumulatedItemIndexStart = -1;

FAccumulated::FAccumulated(const UObject& Context, const FString& InAccumulatedName)
{
    this->AccumulatedIndex = Context.GetWorld()->GetGameInstance()->GetSubsystem<UVoxelSubsystem>()->GetSafeAccumulatedIndex(InAccumulatedName);
    /* We can directly set this to one, as the index may never be an ECommonAccumulated::Null. */
    this->Amount           = 1;

    return;
}

FAccumulated::FAccumulated(const UObject& Context, const FString& InAccumulatedNamespace, const FString& InAccumulatedName)
{
    this->AccumulatedIndex = Context.GetWorld()->GetGameInstance()->GetSubsystem<UVoxelSubsystem>()->GetSafeAccumulatedIndex(InAccumulatedNamespace, InAccumulatedName);
    /* We can directly set this to one, as the index may never be an ECommonAccumulated::Null. */
    this->Amount           = 1;

    return;
}

FAccumulated::FAccumulated(const UObject& Context, const FString& InAccumulatedName, const accamount_t InAmount)
{
    this->AccumulatedIndex = Context.GetWorld()->GetGameInstance()->GetSubsystem<UVoxelSubsystem>()->GetSafeAccumulatedIndex(InAccumulatedName);
    this->Amount           = InAmount;

    return;
}

FAccumulated::FAccumulated(const UObject& Context, const FString& InAccumulatedNamespace, const FString& InAccumulatedName, const accamount_t InAmount)
{
    this->AccumulatedIndex = Context.GetWorld()->GetGameInstance()->GetSubsystem<UVoxelSubsystem>()->GetSafeAccumulatedIndex(InAccumulatedNamespace, InAccumulatedName);
    this->Amount           = InAmount;

    return;
}

FString FAccumulated::GetDisplayName(const UObject* const Context) const
{
    return Context->GetWorld()->GetGameInstance()->GetSubsystem<UVoxelSubsystem>()->GetAccumulatedName(this->AccumulatedIndex);
}
