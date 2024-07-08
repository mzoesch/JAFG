// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGModSubsystem.h"

#include "JAFGLogDefs.h"
#include "ModificationSupervisorSubsystem.h"
#include "JAFGModTypes.h"
#include "MaskFactory.h"

void UJAFGModSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_DISPLAY(LogModSubsystem, "Called.")

    UModificationSupervisorSubsystem* ModSupervisor = this->GetGameInstance()->GetSubsystem<UModificationSupervisorSubsystem>();
    check( ModSupervisor )

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

// ReSharper disable once CppMemberFunctionMayBeStatic
void UJAFGModSubsystem::OnOptionalVoxelsInitialize(TArray<FVoxelMask>& VoxelMasks)
{
    LOG_DISPLAY(LogModSubsystem, "JAFG mod is initializing its voxels.")

    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("Stone")));
    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("Dirt")));
    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("Grass")));
    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("OakPlanks")));
    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("OakLog")));
    VoxelMasks.Add(FVoxelMask(JAFGModNamespace, TEXT("CraftingTable")));

    VoxelMasks.Add(CreateMask_Barrel());

    return;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UJAFGModSubsystem::OnOptionalItemsInitialize(TArray<FItemMask>& ItemMasks)
{
    LOG_DISPLAY(LogModSubsystem, "JAFG mod is initializing its items.")

    ItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenStick")));
    ItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenAxe")));
    ItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenHoe")));
    ItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenPickaxe")));
    ItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenShovel")));
    ItemMasks.Add(FItemMask(JAFGModNamespace, TEXT("WoodenSword")));

    return;
}
