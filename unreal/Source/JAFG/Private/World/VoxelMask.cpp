#include "World/VoxelMask.h"

FVoxelMask::FVoxelMask(const FString& NameSpace, const FString& Name, const bool bIsTranslucent, const int TextureGroup)
{
    this->NameSpace         = NameSpace;
    this->Name              = Name;
    this->bIsTranslucent    = bIsTranslucent;
    this->TextureGroup      = TextureGroup;

    return;
}
