// © 2023 mzoesch. All rights reserved.

#include "Core/GI_Master.h"

#include "Engine/Texture2DArray.h"

#include "Assets/General.h"
#include "World/VoxelMask.h"
#include "World/WorldVoxel.h"

#include "World/Voxels/StoneVoxel.h"
#include "World/Voxels/DirtVoxel.h"
#include "World/Voxels/GrassVoxel.h"
#include "World/Voxels/OakLogVoxel.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

void UGI_Master::Init()
{
    Super::Init();

    FGeneral::Init(this);
    
    this->InitializeVoxels();
    UIL_LOG(Warning, TEXT("Initialized %d voxel masks."), this->VoxelMasks.Num());

    this->InitializeMaterials();
    
    return;
}

void UGI_Master::AddVoxelMask(const FVoxelMask& VoxelMask)
{
    this->VoxelMasks.Add(VoxelMask);
    return;
}

void UGI_Master::InitializeVoxels()
{
    this->VoxelMasks.Add(FVoxelMask("CORE", "NullVoxel", true, -1, nullptr));
    this->VoxelMasks.Add(FVoxelMask("CORE", "AirVoxel", true, -1, nullptr));

    NewObject<UStoneVoxel>()    ->Initialize(this);
    NewObject<UDirtVoxel>()     ->Initialize(this);
    NewObject<UGrassVoxel>()    ->Initialize(this);
    NewObject<UOakLogVoxel>()   ->Initialize(this);
    
    return;
}

void UGI_Master::InitializeMaterials()
{
    this->MDynamicOpaque = UMaterialInstanceDynamic::Create(this->MOpaque, this);

    UTexture2DArray* TexArr     = UTexture2DArray::CreateTransient(16, 16, 3 /* this->GetCoreVoxelNum() */, PF_R8G8B8A8);
    TexArr->Filter              = TF_Nearest;
    TexArr->SRGB                = true;
    TexArr->CompressionSettings = TextureCompressionSettings::TC_Default;

    int TextureIndex = 0;
    for (int i = this->GetCoreVoxelNum(); i < this->VoxelMasks.Num(); ++i)
    {
        if (FGeneral::ExistsAssetTexture2D(this->VoxelMasks[i].NameSpace, this->VoxelMasks[i].Name, ENormalLookup::NL_Top))
        {
            TexArr->SourceTextures.Add(FGeneral::LoadAssetTexture2D(this->VoxelMasks[i].NameSpace, this->VoxelMasks[i].Name, ENormalLookup::NL_Top));
            this->VoxelMasks[i].NormalLookup.Add(ENormalLookup::NL_Top, TextureIndex);
            UIL_LOG(Warning, TEXT("UGI_Master::InitializeMaterials - Added asset texture to array: %s [Normal: %d] with index [%d]."), *this->VoxelMasks[i].Name, ENormalLookup::NL_Top, TextureIndex)
            TextureIndex++;
        }
        
        if (FGeneral::ExistsAssetTexture2D(this->VoxelMasks[i].NameSpace, this->VoxelMasks[i].Name, ENormalLookup::NL_Default))
        {
            TexArr->SourceTextures.Add(FGeneral::LoadAssetTexture2D(this->VoxelMasks[i].NameSpace, this->VoxelMasks[i].Name, ENormalLookup::NL_Default));
            this->VoxelMasks[i].NormalLookup.Add(ENormalLookup::NL_Default, TextureIndex);
            UIL_LOG(Warning, TEXT("UGI_Master::InitializeMaterials - Added asset texture to array: %s [Normal: %d] with index [%d]."), *this->VoxelMasks[i].Name, ENormalLookup::NL_Default, TextureIndex)
            TextureIndex++;
            continue;
        }
    
        UIL_LOG(Error, TEXT("GI_Master::InitializeMaterials - Asset texture does not exists: %s"), *this->VoxelMasks[i].Name);
        
        continue;
    }

    TexArr->UpdateResource();
    TexArr->UpdateSourceFromSourceTextures();

    UE_LOG(LogTemp, Warning, TEXT("UGI_Master::InitializeMaterials - Texture array size: %d"), TexArr->SourceTextures.Num());
    
    this->MDynamicOpaque->SetTextureParameterValue("TexArr", TexArr);

    return;
}

bool UGI_Master::IsVoxelTranslucent(const int Voxel) const
{
    return this->VoxelMasks[Voxel].bIsTranslucent;
}

int UGI_Master::GetTextureIndex(const int Voxel, const FVector& Normal) const
{
    return this->VoxelMasks[Voxel].GetTextureIndex(Normal);
    
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

#undef UIL_LOG
