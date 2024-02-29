// © 2023 mzoesch. All rights reserved.

#include "Core/GI_Master.h"

#include "Engine/Texture2DArray.h"

#include "Assets/General.h"
#include "Lib/DerivedClassHolder.h"
#include "World/VoxelMask.h"

#include "World/Voxels/GrassVoxel.h"
#include "World/Voxels/CraftingTableVoxel.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

void UGI_Master::Init()
{
    Super::Init();

    /* Forward initializations. */
    FGeneral::Init(this);

    this->DerivedClassHolder = NewObject<UDerivedClassHolder>(this, this->DerivedClassHolderClass);
    
    this->InitializeVoxels();
    UIL_LOG(Warning, TEXT("UGI_Master::Init: Successfully initialized %d voxel masks."), this->VoxelMasks.Num());

    this->InitializeItems();
    UIL_LOG(Warning, TEXT("UGI_Master::Init: Successfully initialized %d item masks."), this->ItemMasks.Num());
    
    this->InitializeMaterials();

    UIL_LOG(Warning, TEXT("UGI_Master::Init: Successfully initialized %d [%d - %d # %d] accumulateable items."), this->VoxelMasks.Num() + this->ItemMasks.Num(), this->VoxelMasks.Num(), this->GetCoreVoxelNum(), this->ItemMasks.Num()); 

    /* Post instance initialization initializations. */
    FAccumulated::Init(this);
    
    return;
}

void UGI_Master::InitializeVoxels(void)
{
    this->VoxelMasks.Add(FVoxelMask::NullVoxelMask);
    this->VoxelMasks.Add(FVoxelMask("CORE", "AirVoxel", -1, nullptr));

    this->VoxelMasks.Add(FVoxelMask("JAFG", "StoneVoxel", 0, nullptr));
    this->VoxelMasks.Add(FVoxelMask("JAFG", "DirtVoxel", 0, nullptr));
    NewObject<UGrassVoxel>()->Initialize(this);
    this->VoxelMasks.Add(FVoxelMask("JAFG", "OakLogVoxel", 0, nullptr));
    this->VoxelMasks.Add(FVoxelMask("JAFG", "OakPlanksVoxel", 0, nullptr));
    NewObject<UCraftingTableVoxel>()->Initialize(this);
    
    return;
}

void UGI_Master::InitializeItems(void)
{
    this->ItemMasks.Add(FItemMask("JAFG", "Stick"));
    this->ItemMasks.Add(FItemMask("JAFG", "WoodenAxe"));
    this->ItemMasks.Add(FItemMask("JAFG", "WoodenHoe"));
    this->ItemMasks.Add(FItemMask("JAFG", "WoodenPickaxe"));
    this->ItemMasks.Add(FItemMask("JAFG", "WoodenShovel"));
    this->ItemMasks.Add(FItemMask("JAFG", "WoodenSword"));
    
    return;
}

void UGI_Master::InitializeVoxelMaterials(void)
{
    check( this->MOpaque      )
    check( this->MBlendOpaque )
    check( this->MOpaquePOV   )
    check( this->MItem        )
    check( this->MItemPOV     )
    
    this->MDynamicOpaque        = UMaterialInstanceDynamic::Create(this->MOpaque, this);
    this->MDynamicBlendOpaque   = UMaterialInstanceDynamic::Create(this->MBlendOpaque, this);
    this->MDynamicOpaquePOV     = UMaterialInstanceDynamic::Create(this->MOpaquePOV, this);
    
    UTexture2DArray* TexArr     = UTexture2DArray::CreateTransient(16, 16, /* What is the point of the In Size? */ 1, PF_R8G8B8A8);
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
            UIL_LOG(Log, TEXT("UGI_Master::InitializeMaterials: Added asset texture to array: %s [Normal: %d] with index [%d]."), *this->VoxelMasks[i].Name, ENormalLookup::NL_Top, TextureIndex)
            TextureIndex++;
        }
        
        if (FGeneral::ExistsAssetTexture2D(this->VoxelMasks[i].NameSpace, this->VoxelMasks[i].Name, ENormalLookup::NL_Default))
        {
            TexArr->SourceTextures.Add(FGeneral::LoadAssetTexture2D(this->VoxelMasks[i].NameSpace, this->VoxelMasks[i].Name, ENormalLookup::NL_Default));
            this->VoxelMasks[i].NormalLookup.Add(ENormalLookup::NL_Default, TextureIndex);
            UIL_LOG(Log, TEXT("UGI_Master::InitializeMaterials: Added asset texture to array: %s [Normal: %d] with index [%d]."), *this->VoxelMasks[i].Name, ENormalLookup::NL_Default, TextureIndex)
            TextureIndex++;
            continue;
        }
    
        UIL_LOG(Fatal, TEXT("GI_Master::InitializeMaterials: Asset texture does not exists: %s"), *this->VoxelMasks[i].Name);
        
        return;;
    }

    TexArr->UpdateResource();
    /* OHOH Why is this EDITOR ONLY ?? Shit ? */
    TexArr->UpdateSourceFromSourceTextures();

    this->MDynamicOpaque->SetTextureParameterValue("TexArr", TexArr);
    this->MDynamicBlendOpaque->SetTextureParameterValue("TexArr", TexArr);
    this->MDynamicOpaquePOV->SetTextureParameterValue("TexArr", TexArr);
    
    UIL_LOG(Log, TEXT("UGI_Master::InitializeMaterials: Texture array size: %d"), TexArr->SourceTextures.Num());

    return;
}

void UGI_Master::InitializeMaterials()
{
    this->InitializeVoxelMaterials();
    return;
}

int UGI_Master::GetCoreVoxelNum() const
{
    int Res = 0;
    
    for (int i = 0; i < this->VoxelMasks.Num(); ++i)
    {
        if (this->VoxelMasks[i].NameSpace == "CORE")
        {
            Res++;
            continue;
        }

        break;
    }

    return Res;
}

FAccumulated UGI_Master::GetAccumulatedByName(const FString& Name) const
{
    for (int i = 0; i < this->VoxelMasks.Num(); ++i)
    {
        if (this->VoxelMasks[i].Name == Name)
        {
            return FAccumulated(i, 1);
        }

        continue;
    }

    for (int i = 0; i < this->ItemMasks.Num(); ++i)
    {
        if (this->ItemMasks[i].Name == Name)
        {
            return FAccumulated(this->GetItemIndexStart() + i, 1);
        }

        continue;
    }
    
#if WITH_EDITOR
    UIL_LOG(Error, TEXT("UGI_Master::GetAccumulatedByName: Accumulated with name '%s' was not found."), *Name);
#else
    UIL_LOG(Fatal, TEXT("UGI_Master::GetAccumulatedByName: Accumulated with name '%s' was not found."), *Name);
#endif /* WITH_EDITOR */
    
    return FAccumulated::NullAccumulated;
}

#undef UIL_LOG
