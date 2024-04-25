// Copyright 2024 mzoesch. All rights reserved.

#include "World/Voxel/VoxelMask.h"

#include "System/MaterialSubsystem.h"
#include "World/Voxel/CommonVoxels.h"

const FVoxelMask FVoxelMask::Null = FVoxelMask(TEXT("COMMON"), TEXT("NullVoxel"), ETextureGroup::Core);
const FVoxelMask FVoxelMask::Air  = FVoxelMask(TEXT("COMMON"), TEXT("AirVoxel"),  ETextureGroup::Core);

FVoxelMask::FVoxelMask(const FString& NameSpace, const FString& Name, const ETextureGroup::Type TextureGroup)
{
    this->NameSpace = NameSpace;
    this->Name      = Name;

    this->TextureGroups.Empty();
    this->TextureGroups.Add(FTextureGroup(ENormalLookup::Default, TextureGroup));

    this->TextureIndices.Empty();

    return;
}

FVoxelMask::FVoxelMask(const FString& NameSpace, const FString& Name, const TMap<ENormalLookup::Type, ETextureGroup::Type>& TextureGroup)
{
    this->NameSpace = NameSpace;
    this->Name      = Name;

    this->TextureGroups.Empty();
    const ETextureGroup::Type* DefaultGroup = TextureGroup.Find(ENormalLookup::Default);
    if (DefaultGroup == nullptr)
    {
        LOG_FATAL(LogVoxelSubsystem, "Default group not found in Texture Group map. Faulty Mask: %s::%s.", *NameSpace, *Name)
        return;
    }
    for (const TPair<ENormalLookup::Type, ETextureGroup::Type>& Pair : TextureGroup)
    {
        if (Pair.Key == ENormalLookup::Default)
        {
            continue;
        }

        this->TextureGroups.Add(FTextureGroup(Pair.Key, Pair.Value));
    }
    this->TextureGroups.Add(FTextureGroup(ENormalLookup::Default, *DefaultGroup));

    /* Initialized later on. See the MaterialSubsystem.h for more information. */
    this->TextureIndices.Empty();

    return;
}

FVoxelMask::FTextureGroup::FTextureGroup(const ENormalLookup::Type Normal, const ETextureGroup::Type TextureGroup)
{
    this->Normal       = Normal;
    this->TextureGroup = TextureGroup;
}

FVoxelMask::FTextureIndex::FTextureIndex(const ENormalLookup::Type Normal, const int32 TextureIndex)
{
    this->Normal = Normal;
    this->TextureIndex = TextureIndex;
}

ETextureGroup::Type FVoxelMask::GetTextureGroup(const FVector& Normal) const
{
    const ENormalLookup::Type NormalLookup = ENormalLookup::FromVector(Normal);

    for (const FTextureGroup& TextureGroup : this->TextureGroups)
    {
        if (TextureGroup.Normal == NormalLookup)
        {
            return TextureGroup.TextureGroup;
        }
    }

    return this->TextureGroups[this->TextureGroups.Num() - 1].TextureGroup;
}

int32 FVoxelMask::GetTextureIndex(const FVector& Normal) const
{
    const ENormalLookup::Type NormalLookup = ENormalLookup::FromVector(Normal);

    for (const FTextureIndex& TextureIndex : this->TextureIndices)
    {
        if (TextureIndex.Normal == NormalLookup)
        {
            return TextureIndex.TextureIndex;
        }
    }

    return this->TextureIndices[this->TextureIndices.Num() - 1].TextureIndex;
}
