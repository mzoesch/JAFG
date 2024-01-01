#include "World/VoxelMask.h"

FVoxelMask::FVoxelMask(const FString& NameSpace, const FString& Name, const bool bIsTranslucent)
{
    this->NameSpace         = NameSpace;
    this->Name              = Name;
    this->bIsTranslucent    = bIsTranslucent;

    return;
}
