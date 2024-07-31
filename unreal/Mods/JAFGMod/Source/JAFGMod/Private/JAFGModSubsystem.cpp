// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGModSubsystem.h"
#include "JAFGLogDefs.h"
#include "MaskFactory.h"

void UJAFGModSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_DISPLAY(LogModSubsystem, "Called.")

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UJAFGModSubsystem::OnOptionalVoxelsInitialize(TArray<FVoxelMask>& OutVoxelMasks)
{
    Super::OnOptionalVoxelsInitialize(OutVoxelMasks);

    LOG_DISPLAY(LogModSubsystem, "JAFG mod is initializing its voxels.")

    OutVoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("Stone")));
    OutVoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("Dirt")));
    OutVoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("Grass")));
    OutVoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("OakPlanks")));
    OutVoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("OakLog")));
    OutVoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("CraftingTable")));

    OutVoxelMasks.Add(CreateMask_Barrel());

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UJAFGModSubsystem::OnOptionalItemsInitialize(TArray<FItemMask>& OutItemMasks)
{
    Super::OnOptionalItemsInitialize(OutItemMasks);

    LOG_DISPLAY(LogModSubsystem, "JAFG mod is initializing its items.")

    OutItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenStick")));
    OutItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenAxe")));
    OutItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenHoe")));
    OutItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenPickaxe")));
    OutItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenShovel")));
    OutItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenSword")));

    return;
}
