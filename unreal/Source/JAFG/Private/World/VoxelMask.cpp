#include "World/VoxelMask.h"

#include "Assets/General.h"
#include "World/Voxels/Voxel.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

const FVoxelMask FVoxelMask::NullVoxelMask = FVoxelMask("CORE", "NullVoxel", -1, nullptr);

FVoxelMask::FVoxelMask(const FString& NameSpace, const FString& Name, const int TextureGroup, TScriptInterface<IVoxel>* VoxelClass)
{
    this->NameSpace         = NameSpace;
    this->Name              = Name;
    this->TextureGroup.Empty();
    this->TextureGroup.Add(ENormalLookup::NL_Default, TextureGroup);
    this->VoxelClass        = VoxelClass ? *VoxelClass : nullptr;
    this->NormalLookup.Empty();}

FVoxelMask::FVoxelMask(const FString& NameSpace, const FString& Name, const TMap<ENormalLookup, int>& TextureGroup, TScriptInterface<IVoxel>* VoxelClass)
{
    this->NameSpace         = NameSpace;
    this->Name              = Name;
    this->TextureGroup.Empty();
    for (const TPair<ENormalLookup, int>& Pair : TextureGroup)
    { this->TextureGroup.Add(Pair.Key, Pair.Value); }
    if (this->TextureGroup.Contains(ENormalLookup::NL_Default) == false)
    { this->TextureGroup.Add(ENormalLookup::NL_Default, 0); }
    this->VoxelClass        = VoxelClass ? *VoxelClass : nullptr;
    this->NormalLookup.Empty();

    return;
}

int FVoxelMask::GetTextureIndex(const FVector& Normal) const
{
    if (const ENormalLookup Value = FGeneral::GetNormalLookup(Normal); this->NormalLookup.Contains(Value))
    {
        return this->NormalLookup[Value];
    }

    if (this->NormalLookup.Contains(ENormalLookup::NL_Default))
    {
        return this->NormalLookup[ENormalLookup::NL_Default];
    }

    UIL_LOG(Error, TEXT("FVoxelMask::GetTextureIndex: Failed to find texture index for voxel: %s::%s."), *this->NameSpace, *this->Name);
    return 0;
}

int FVoxelMask::GetTextureGroup(const FVector& Normal) const
{
    if (const ENormalLookup Value = FGeneral::GetNormalLookup(Normal); this->TextureGroup.Contains(Value))
    {
        return this->TextureGroup[Value];
    }

    if (this->TextureGroup.Contains(ENormalLookup::NL_Default))
    {
        return this->TextureGroup[ENormalLookup::NL_Default];
    }

    UIL_LOG(Error, TEXT("FVoxelMask::GetTextureGroup: Failed to find texture group for voxel: %s::%s. %s"), *this->NameSpace, *this->Name, *Normal.ToString())
    return 0;
    // return this->TextureGroup[ENormalLookup::NL_Default];
}

#undef UIL_LOG
