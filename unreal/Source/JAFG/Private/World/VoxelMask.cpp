#include "World/VoxelMask.h"

FVoxelMask::FVoxelMask(const FString& NameSpace, const FString& Name, const bool bIsTranslucent, const int TextureGroup, class IVoxel* VoxelClass)
{
    this->NameSpace         = NameSpace;
    this->Name              = Name;
    this->bIsTranslucent    = bIsTranslucent;
    this->TextureGroup      = TextureGroup;
    this->VoxelClass        = VoxelClass;

    return;
}
