// Copyright 2024 mzoesch. All rights reserved.

#include "System/MaterialSubsystem.h"

#include "Engine/Texture2DArray.h"
#include "System/JAFGInstance.h"
#include "World/Voxel/VoxelSubsystem.h"
#include "ImageUtils.h"

void UMaterialSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UVoxelSubsystem>();
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Log, TEXT("UMaterialSubsystem::Initialize: Initializing Material Subsystem."))

    this->InitializeMaterials();

    return;
}

void UMaterialSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

void UMaterialSubsystem::InitializeMaterials(void)
{
    if (this->GetWorld() == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UMaterialSubsystem::InitializeMaterials: World is nullptr."))
        return;
    }

    const UJAFGInstance* JAFGInstance = Cast<UJAFGInstance>(GetWorld()->GetGameInstance());
    if (JAFGInstance == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UMaterialSubsystem::InitializeMaterials: Could not cast UJAFGInstance."))
        return;
    }

    this->MDynamicOpaque           = UMaterialInstanceDynamic::Create(JAFGInstance->MOpaque, this);
    this->MDynamicFullBlendOpaque  = UMaterialInstanceDynamic::Create(JAFGInstance->MFullBlendOpaque, this);
    this->MDynamicFloraBlendOpaque = UMaterialInstanceDynamic::Create(JAFGInstance->MFloraBlendOpaque, this);

    UTexture2DArray* TextureArray = UTexture2DArray::CreateTransient(UMaterialSubsystem::TextureArrayWidthHorizontal, UMaterialSubsystem::TextureArrayWidthVertical, 4, EPixelFormat::PF_R8G8B8A8);
    TextureArray->Filter = TextureFilter::TF_Nearest;
    TextureArray->SRGB = true;
    TextureArray->CompressionSettings = TextureCompressionSettings::TC_Default;

    UVoxelSubsystem* VoxelSubsystem = JAFGInstance->GetSubsystem<UVoxelSubsystem>();
    if (!VoxelSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("UMaterialSubsystem::InitializeMaterials: Could not get UVoxelSubsystem."))
        return;
    }

    // Write straight to bulk data
    void* TextureArrayMipData = TextureArray->GetPlatformData()->Mips[0].BulkData.Lock( LOCK_READ_WRITE );
    int64 iCurrentMemoryOffset = 0;
    uint32 iSliceSize = 16 * 16 * 4; /*  SizeX * SizeY * FormatDataSize; */

    for( int32 SourceTexIndex = 0; SourceTexIndex < 4 /* m_RuntimeSourceTextures.Num() */; ++SourceTexIndex )
    {
        // Offset write location by size of previously written textures
        void* pDestSliceData = (uint8*)TextureArrayMipData + iCurrentMemoryOffset;

        // TODO ...
        // R G B
        // But somehow it reads as B G R
        // We need to read in the other way around

        // Find raw image data in source texture
        UTexture2D* pSliceTexture =
            SourceTexIndex == 0 ? FImageUtils::ImportFileAsTexture2D( TEXT("E:/dev/ue/prj_bi/JAFGv3/unreal/Content/Assets/Textures/Voxels/JAFG/R.png") ) :
            SourceTexIndex == 1 ? FImageUtils::ImportFileAsTexture2D( TEXT("E:/dev/ue/prj_bi/JAFGv3/unreal/Content/Assets/Textures/Voxels/JAFG/G.png") ) :
            SourceTexIndex == 2 ? FImageUtils::ImportFileAsTexture2D( TEXT("E:/dev/ue/prj_bi/JAFGv3/unreal/Content/Assets/Textures/Voxels/JAFG/B.png") ) :
            SourceTexIndex == 3 ? FImageUtils::ImportFileAsTexture2D( TEXT("E:/dev/ue/prj_bi/JAFGv3/unreal/Content/Assets/Textures/Voxels/JAFG/crafting_table_side.png") ) :
            nullptr;
            // m_RuntimeSourceTextures[SourceTexIndex];

        ensureAlwaysMsgf( pSliceTexture, TEXT( "Missing source texture while making texture array, element: %d" ), SourceTexIndex );
        const void* pSourceMipData = pSliceTexture->GetPlatformMips()[0].BulkData.LockReadOnly();
        ensureAlwaysMsgf( pSourceMipData, TEXT( "Missing source texture while making texture array, element: %d" ), SourceTexIndex );

        // Copy from source texture to texture array
        FMemory::Memcpy( pDestSliceData, pSourceMipData, iSliceSize );

        // Unlock source texture
        if (pSliceTexture->GetPlatformData()->Mips[0].BulkData.IsLocked())
        {
            pSliceTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
        }

        // Increase memory offset for writing
        iCurrentMemoryOffset += iSliceSize;
    }
    TextureArray->GetPlatformData()->Mips[0].BulkData.Unlock();

    TextureArray->UpdateResource();

#if 1
    int TextureIndex = 0;
    constexpr int StoneVoxelIdx = 2; /* two core voxels */
    constexpr int DirtVoxelIdx = 3;
    constexpr int GrassVoxelIdx = 4;

    // TextureArray->SourceTextures.Add(JAFGInstance->Stone);
    VoxelSubsystem->VoxelMasks[StoneVoxelIdx].AddTextureIndex(ENormalLookup::Default, 0);

    // TextureArray->SourceTextures.Add(JAFGInstance->Dirt);
    VoxelSubsystem->VoxelMasks[DirtVoxelIdx].AddTextureIndex(ENormalLookup::Default, 1);

    VoxelSubsystem->VoxelMasks[GrassVoxelIdx].AddTextureIndex(ENormalLookup::Bottom, 1);
    // TextureArray->SourceTextures.Add(JAFGInstance->GrassTop);
    VoxelSubsystem->VoxelMasks[GrassVoxelIdx].AddTextureIndex(ENormalLookup::Top, 2);
    // TextureArray->SourceTextures.Add(JAFGInstance->Grass);
    VoxelSubsystem->VoxelMasks[GrassVoxelIdx].AddTextureIndex(ENormalLookup::Default, 3);

    // https://forums.unrealengine.com/t/cant-make-a-a-texture-array-at-runtime/157889/2

    // TextureArray->UpdateResource();
    // TextureArray->UpdateSourceFromSourceTextures();
#endif

    this->MDynamicOpaque->SetTextureParameterValue("TexArr", TextureArray);
    this->MDynamicFullBlendOpaque->SetTextureParameterValue("TexArr", TextureArray);
    this->MDynamicFloraBlendOpaque->SetTextureParameterValue("TexArr", TextureArray);

    UE_LOG(LogTemp, Log, TEXT("UMaterialSubsystem::InitializeMaterials: Texture array size: %d"), TextureArray->SourceTextures.Num());

    return;
}
