// Copyright 2024 mzoesch. All rights reserved.

#include "System/MaterialSubsystem.h"

#include "JAFGMaterialSettings.h"
#include "Engine/Texture2DArray.h"
#include "System/JAFGGameInstance.h"
#include "System/VoxelSubsystem.h"
#include "System/TextureSubsystem.h"
#include "JAFGMaterialSettings.h"

UMaterialSubsystem::UMaterialSubsystem(void) : Super()
{
    return;
}

void UMaterialSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UVoxelSubsystem>();
    Collection.InitializeDependency<UTextureSubsystem>();
    Super::Initialize(Collection);

    LOG_VERBOSE(LogMaterialSubsystem, "Called.")

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

    const UJAFGGameInstance* JAFGInstance = Cast<UJAFGGameInstance>(GetWorld()->GetGameInstance());
    if (JAFGInstance == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UMaterialSubsystem::InitializeMaterials: Could not cast UJAFGInstance."))
        return;
    }

    const UJAFGMaterialSettings* MaterialSettings = GetDefault<UJAFGMaterialSettings>();
    if (MaterialSettings == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("UMaterialSubsystem::InitializeMaterials: Could not get UJAFGMaterialSettings."))
        return;
    }

    this->MDynamicOpaque           = UMaterialInstanceDynamic::Create(MaterialSettings->MOpaque.LoadSynchronous(), this);
    this->MDynamicFullBlendOpaque  = UMaterialInstanceDynamic::Create(MaterialSettings->MFullBlendOpaque.LoadSynchronous(), this);
    this->MDynamicFloraBlendOpaque = UMaterialInstanceDynamic::Create(MaterialSettings->MFloraBlendOpaque.LoadSynchronous(), this);

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

    UTextureSubsystem* TextureSubsystem = JAFGInstance->GetSubsystem<UTextureSubsystem>();
    check( TextureSubsystem )

    // Write straight to bulk data
    void* TextureArrayMipData = TextureArray->GetPlatformData()->Mips[0].BulkData.Lock( LOCK_READ_WRITE );
    int64 iCurrentMemoryOffset = 0;
    uint32 iSliceSize = 16 * 16 * 4; /*  SizeX * SizeY * FormatDataSize; */
    uint32 iSinglePixelSize = 4;

    for ( int32 SourceTexIndex = 0; SourceTexIndex < 4 /* m_RuntimeSourceTextures.Num() */; ++SourceTexIndex )
    {
        // Offset write location by size of previously written textures
        void* pDestSliceData = (uint8*)TextureArrayMipData + iCurrentMemoryOffset;

        //
        // Okay, so there is a small problem with the RGBA format.
        // The source texture is in B8G8R8A8 format, but the texture array is in R8G8B8A8 format.
        // So what we have to do is to flip the red and blue channels.
        // We do this directly during the memcpy operation. We do not want to modify the bulk data of the source
        // texture directly, because they are used elsewhere (mainly in the HUD and OSD) where the format is correct.
        //


        UTexture2D* pSliceTexture =
            SourceTexIndex == 0 ? TextureSubsystem->GetWorldTexture2D( "JAFG", "StoneVoxel" ) :
            SourceTexIndex == 1 ? TextureSubsystem->GetWorldTexture2D( "JAFG", "DirtVoxel" ) :
            SourceTexIndex == 2 ? TextureSubsystem->GetWorldTexture2D( "JAFG", "GrassVoxelTop" ) :
            SourceTexIndex == 3 ? TextureSubsystem->GetWorldTexture2D( "JAFG", "GrassVoxel" ) :
            nullptr;

        ensureAlwaysMsgf( pSliceTexture, TEXT( "Missing source texture while making texture array, element: %d" ), SourceTexIndex );
        const void* pSourceMipData = pSliceTexture->GetPlatformMips()[0].BulkData.LockReadOnly();
        ensureAlwaysMsgf( pSourceMipData, TEXT( "Missing source texture while making texture array, element: %d" ), SourceTexIndex );

        // Copy from source texture to the texture array

        for (int channel = 0; channel < 4; channel++)
        {
            for (int i = 0; i < 16*16; i++)
            {
                /*
                 * This actually works here (somehow) on my machine. But if this causes problems on other machines, we
                 * may want to use the FMemory::Memcpy function instead.
                 */
                ((uint8*)pDestSliceData)[i * 4 + channel] = ((uint8*)pSourceMipData)[i * 4 + (channel == 0 ? 2 : channel == 2 ? 0 : channel)];
            }
        }

#if LOG_TEX_ARR_CHANNEL_FLIPS
        for (int i = 0; i < 16*16; i++)
        {
            UE_LOG(LogTemp, Log, TEXT("px:%d; %d %d %d %d --- %d %d %d %d"),
                i,
                ((uint8*)pSourceMipData)[i * 4 + 0],
                ((uint8*)pSourceMipData)[i * 4 + 1],
                ((uint8*)pSourceMipData)[i * 4 + 2],
                ((uint8*)pSourceMipData)[i * 4 + 3],
                ((uint8*)pDestSliceData)[i * 4 + 0],
                ((uint8*)pDestSliceData)[i * 4 + 1],
                ((uint8*)pDestSliceData)[i * 4 + 2],
                ((uint8*)pDestSliceData)[i * 4 + 3])
        }
#endif /* LOG_TEX_ARR_CHANNEL_FLIPS */

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

    // UE_LOG(LogTemp, Log, TEXT("UMaterialSubsystem::InitializeMaterials: Texture array size: %d"), TextureArray->SourceTextures.Num());

    return;
}
