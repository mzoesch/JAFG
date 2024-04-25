// Copyright 2024 mzoesch. All rights reserved.

#include "GrassVoxel.h"

#include "Kismet/GameplayStatics.h"
#include "World/Voxel/VoxelMask.h"
#include "System/MaterialSubsystem.h"
#include "World/Voxel/VoxelSubsystem.h"

class UVoxelSubsystem;

void UGrassVoxel::Initialize(void)
{
    UVoxelSubsystem* OwningSubsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UVoxelSubsystem>();
    check( OwningSubsystem )

    TMap<ENormalLookup::Type, ETextureGroup::Type> GrassTextureGroups;
    GrassTextureGroups.Add(ENormalLookup::Default, ETextureGroup::FloraBlendOpaque);
    GrassTextureGroups.Add(ENormalLookup::Top, ETextureGroup::FullBlendOpaque);
    GrassTextureGroups.Add(ENormalLookup::Bottom, ETextureGroup::Opaque);
    OwningSubsystem->AddOptionalVoxelDuringInitialization(FVoxelMask(TEXT("JAFG"), TEXT("Grass"), GrassTextureGroups));

    return;
}
