#include "World/VoxelMask.h"

#include "Assets/General.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

FVoxelMask::FVoxelMask(const FString& NameSpace, const FString& Name, const bool bIsTranslucent, const int TextureGroup, class IVoxel* VoxelClass)
{
    this->NameSpace         = NameSpace;
    this->Name              = Name;
    this->bIsTranslucent    = bIsTranslucent;
    this->TextureGroup      = TextureGroup;
    this->VoxelClass        = VoxelClass;
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

    UIL_LOG(Error, TEXT("FVoxelMask::GetTextureIndex - Failed to find texture index for voxel: %s"), *this->Name);
    return 0;
}

#undef UIL_LOG
