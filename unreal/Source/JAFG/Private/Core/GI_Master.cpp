// © 2023 mzoesch. All rights reserved.

#include "Core/GI_Master.h"

#include "Assets/General.h"
#include "World/VoxelMask.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

void UGI_Master::Init()
{
    Super::Init();

    FGeneral::Init(this);
    
    this->InitializeVoxels();
    UIL_LOG(Warning, TEXT("Initialized %d voxel masks."), this->VoxelMasks.Num());

    return;
}

void UGI_Master::InitializeVoxels()
{
    this->VoxelMasks.Add(FVoxelMask("CORE", "NullVoxel", true, -1));
    this->VoxelMasks.Add(FVoxelMask("CORE", "AirVoxel", true, -1));

    this->VoxelMasks.Add(FVoxelMask("JAFG", "StoneVoxel", false, 0));
    this->VoxelMasks.Add(FVoxelMask("JAFG", "DirtVoxel", false, 0));
    this->VoxelMasks.Add(FVoxelMask("JAFG", "GrassVoxel", false, 0));
    
    return;
}

bool UGI_Master::IsVoxelTranslucent(const int Voxel) const
{
    return this->VoxelMasks[Voxel].bIsTranslucent;
}

int UGI_Master::GetTextureIndex(const int Voxel, const FVector& Normal) const
{
    switch (Voxel)
    {
    case 2: return 0;
    case 3: return 1;
    case 4:
        {
            if (Normal == FVector::DownVector)
            {
                return 1;
            }
            
            if (Normal == FVector::UpVector)
            {
                return 2;
            }

            return 3;
        }
    default: return 255;
    }
}

int UGI_Master::GetTextureGroup(const int Voxel) const
{
    return this->VoxelMasks[Voxel].TextureGroup;
}

FString UGI_Master::GetVoxelName(const int Voxel) const
{
    return this->VoxelMasks[Voxel].Name;
}

int UGI_Master::GetVoxelNum() const
{
    return this->VoxelMasks.Num();
}

int UGI_Master::GetCoreVoxelNum() const
{
    int res = 0;
    
    for (int i = 0; i < this->VoxelMasks.Num(); ++i)
    {
        if (this->VoxelMasks[i].NameSpace == "CORE")
        {
            res++;
            continue;
        }

        break;
    }

    return res;
}

