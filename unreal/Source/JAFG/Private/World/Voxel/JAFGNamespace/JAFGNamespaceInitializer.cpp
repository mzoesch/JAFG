// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGNamespaceInitializer.h"

#include "CraftingTableVoxel.h"
#include "GrassVoxel.h"
#include "Kismet/GameplayStatics.h"
#include "World/Voxel/VoxelSubsystem.h"

#define ADD_V_FMASK(Mask)                                        \
    OwningSubsystem->AddOptionalVoxelDuringInitialization(Mask);
#define ADD_V_CLASS(Name)                                        \
    NewObject<Name>(Outer)->Initialize();

void UJAFGNamespaceInitializer::InitializeOptionalVoxels(UObject* Outer)
{
    LOG_VERBOSE(LogVoxelSubsystem, "Called.")

    UVoxelSubsystem* OwningSubsystem = UGameplayStatics::GetGameInstance(Outer)->GetSubsystem<UVoxelSubsystem>();
    check( OwningSubsystem )

    ADD_V_FMASK(FVoxelMask(PrivateJAFG::SpaceName, TEXT("Stone"), ETextureGroup::Opaque))
    ADD_V_FMASK(FVoxelMask(PrivateJAFG::SpaceName, TEXT("Dirt"), ETextureGroup::Opaque))
    ADD_V_CLASS(UGrassVoxel)
    ADD_V_CLASS(UCraftingTableVoxel)

    return;
}

#undef ADD_V_FMASK
#undef ADD_V_CLASS
