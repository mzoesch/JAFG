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

void UMaterialSubsystem::InitializeAlphaMasks(void)
{
    const UJAFGGameInstance*     MyGameInstance   = Cast<UJAFGGameInstance>(GetWorld()->GetGameInstance());

    if (MyGameInstance == nullptr)
    {
        LOG_FATAL(LogMaterialSubsystem, "Game instance is not of type UJAFGGameInstance.")
        return;
    }

          UTextureSubsystem*     TextureSubsystem = MyGameInstance->GetSubsystem<UTextureSubsystem>();

    if (TextureSubsystem == nullptr)
    {
        LOG_FATAL(LogMaterialSubsystem, "Texture subsystem is invalid.")
        return;
    }

    this->Blends = TextureSubsystem->LoadAllBlendTextureNames();
#if !UE_BUILD_SHIPPING
    if (this->Blends.Num() == 0)
    {
        LOG_ERROR(LogMaterialSubsystem, "No blend textures found. Was this intentional?")
    }
#endif /* !UE_BUILD_SHIPPING */

    return;
}

void UMaterialSubsystem::InitializeMaterials(void)
{
    this->InitializeAlphaMasks();

    const UJAFGGameInstance*     MyGameInstance   = Cast<UJAFGGameInstance>(GetWorld()->GetGameInstance());

    if (MyGameInstance == nullptr)
    {
        LOG_FATAL(LogMaterialSubsystem, "Game instance is not of type UJAFGGameInstance.")
        return;
    }

          UVoxelSubsystem*       VoxelSubsystem   = MyGameInstance->GetSubsystem<UVoxelSubsystem>();
          UTextureSubsystem*     TextureSubsystem = MyGameInstance->GetSubsystem<UTextureSubsystem>();
    const UJAFGMaterialSettings* MaterialSettings = GetDefault<UJAFGMaterialSettings>();

    if (VoxelSubsystem == nullptr)
    {
        LOG_FATAL(LogMaterialSubsystem, "Voxel subsystem is invalid.")
        return;
    }

    if (TextureSubsystem == nullptr)
    {
        LOG_FATAL(LogMaterialSubsystem, "Texture subsystem is invalid.")
        return;
    }

    if (MaterialSettings == nullptr)
    {
        LOG_FATAL(LogMaterialSubsystem, "Material settings are invalid.")
        return;
    }

    TextureSubsystem->LoadTextureNamesForNamespace("JAFG");
    const int32 NamespaceTexCount = TextureSubsystem->GetWorldTexture2DCount("JAFG");
    LOG_VERBOSE(LogMaterialSubsystem, "For namespace %s, there were %d textures found.", *FString("JAFG"), NamespaceTexCount)

    UTexture2DArray* TexArr = UTexture2DArray::CreateTransient(
        UMaterialSubsystem::TexArrWidthHorizontal,
        UMaterialSubsystem::TexArrWidthVertical,
        NamespaceTexCount,
        /* The old pixel format. Look at Tex Channel flips for more information. */
        /* EPixelFormat::PF_R8G8B8A8, */
        EPixelFormat::PF_B8G8R8A8
    );

    TexArr->Filter              = TextureFilter::TF_Nearest;
    TexArr->SRGB                = true;
    TexArr->CompressionSettings = TextureCompressionSettings::TC_Default;

    /*
     * Note that we are using here the pixel format of FColor. But we are actually reading the pixel format of
     * BGRA8. Currently, this is the same number of bits, but we probably should do a proper approach to this value
     * later. Maybe even with checking the files directly to allow for more formats.
     * Same for the width and height of the texture array. We should not hard-code that here.
     */
    const uint32 SinglePixelSize = UTextureSubsystem::GetBytesPerPixel(ERawImageFormat::BGRA8);
    const uint32 SliceSize       =
        UMaterialSubsystem::TexArrWidthHorizontal *
        UMaterialSubsystem::TexArrWidthVertical   *
        SinglePixelSize;

    void* TexArrMipDataPtr          = TexArr->GetPlatformData()->Mips[0].BulkData.Lock( LOCK_READ_WRITE );
    int64 CurrentTexArrMemoryOffset = 0;

    for (int32 SourceTexIndex = 0; SourceTexIndex < NamespaceTexCount; ++SourceTexIndex)
    {
        const FString& CurrentTexName    = TextureSubsystem->GetWorldTexture2DNameByIndex("JAFG", SourceTexIndex);
        int32 CurrentTexBlend = this->NoBlend;

        FString CurrentVoxelName = L"";
        if (CurrentTexName.Contains(TextureSubsystem->TexSectionDivider))
        {
            CurrentVoxelName = CurrentTexName.Left(CurrentTexName.Find(TextureSubsystem->TexSectionDivider));
        }
        else
        {
            CurrentVoxelName = CurrentTexName;
        }

        const voxel_t  CurrentVoxelIndex = VoxelSubsystem->GetVoxelIndex("JAFG", CurrentVoxelName);
        if (CurrentVoxelIndex == ECommonVoxels::Null)
        {
            LOG_FATAL(LogMaterialSubsystem, "Voxel index is invalid for voxel [%s::%s]." , *FString("JAFG"), *CurrentVoxelName)
            return;
        }

        if (CurrentTexName.Contains(TextureSubsystem->TexSectionDivider))
        {
            int32 DividerCount = 0;
            for (int32 i = 0; i < CurrentTexName.Len(); ++i)
            {
                if (CurrentTexName[i] == TextureSubsystem->TexSectionDividerChar)
                {
                    DividerCount++;
                }
            }
            check( DividerCount > 0 )

            // For example, GrassVoxel_Top_Full;

            int32 LastDividerIndex = -1;
            CurrentTexName.FindLastChar(TextureSubsystem->TexSectionDividerChar, LastDividerIndex);
            check( LastDividerIndex != -1 )
            const FString SectionAfterLastDivider = CurrentTexName.Right(CurrentTexName.Len() - LastDividerIndex - 1);

            if (this->Blends.Contains(SectionAfterLastDivider))
            {
                CurrentTexBlend = this->Blends.Find(SectionAfterLastDivider);
            }
            else
            {
                /* Has to have a blend tex. For example, GrassVoxel_Top_Full */
                if (DividerCount == 2)
                {
                    LOG_FATAL(LogMaterialSubsystem, "Blend texture not found for voxel [%s::%s].", *FString("JAFG"), *CurrentTexName)
                }

                // We have something like GrassVoxel_Top

                if (ENormalLookup::IsValid(SectionAfterLastDivider) == false)
                {
                    LOG_FATAL(LogMaterialSubsystem, "Invalid normal lookup for voxel [%s::%s]. Faulty texture: %s.", *FString("JAFG"), *CurrentVoxelName, *CurrentTexName)
                    return;
                }

                ENormalLookup::Type NormalLookup = ENormalLookup::FromString(SectionAfterLastDivider);
                VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddSafeTextureIndex(NormalLookup, SourceTexIndex);
            }

            if (DividerCount == 2)
            {
                // We have something like GrassVoxel_Top_Full
                FString LeftPad = CurrentTexName.Left(LastDividerIndex);
                FString SectionInMiddle = LeftPad.Right(LeftPad.Len() - LeftPad.Find(TextureSubsystem->TexSectionDivider) - 1);
                if (ENormalLookup::IsValid(SectionInMiddle) == false)
                {
                    LOG_FATAL(LogMaterialSubsystem, "Invalid normal lookup for voxel [%s::%s]. Faulty texture: %s.", *FString("JAFG"), *CurrentVoxelName, *CurrentTexName)
                    return;
                }

                ENormalLookup::Type NormalLookup = ENormalLookup::FromString(SectionInMiddle);
                VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddSafeTextureIndex(NormalLookup, SourceTexIndex);
            }
        }
        else // Does not have a _ divider; For example, DirtVoxel
        {
            VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddTextureIndex(ENormalLookup::Default, SourceTexIndex);
        }

        /* Offset write location by size of previously written textures */
        void* DestSliceMipDataPtr = static_cast<uint8*>(TexArrMipDataPtr) + CurrentTexArrMemoryOffset;

        UTexture2D* CurrentSlicePtr = TextureSubsystem->GetWorldTexture2D("JAFG", CurrentTexName);
        if (CurrentSlicePtr == nullptr)
        {
            LOG_FATAL(LogMaterialSubsystem, "Missing source texture while making texture array, element: %d. For %s::%s.", SourceTexIndex, *FString("JAFG"), *CurrentTexName)
            return;;
        }

        const void* CurrentSliceMipDataPtr = CurrentSlicePtr->GetPlatformMips()[0].BulkData.LockReadOnly();
        if (CurrentSliceMipDataPtr == nullptr)
        {
            LOG_FATAL(LogMaterialSubsystem, "Failed to aquire read only lock for source texture while making texture array, element: %d. For %s::%s.", SourceTexIndex, *FString("JAFG"), *CurrentTexName)
            return;
        }

        FMemory::Memcpy( DestSliceMipDataPtr, CurrentSliceMipDataPtr, SliceSize );
        //
        // Okay, so there is a small problem with the RGBA format.
        // The source texture is in B8G8R8A8 format, but the texture array is in R8G8B8A8 format.
        // So what we have to do is to flip the red and blue channels.
        // We do this directly during the memcpy operation. We do not want to modify the bulk data of the source
        // texture directly, because they are used elsewhere (mainly in the HUD and OSD) where the format is correct.
        //
        // for (int channel = 0; channel < 4; channel++)
        // {
        //     for (int i = 0; i < 16*16; i++)
        //     {
        //         /*
        //          * This actually works here (somehow) on my machine. But if this causes problems on other machines, we
        //          * may want to use the FMemory::Memcpy function instead.
        //          */
        //         ((uint8*)pDestSliceData)[i * 4 + channel] = ((uint8*)pSourceMipData)[i * 4 + (channel == 0 ? 2 : channel == 2 ? 0 : channel)];
        //     }
        // }

#if 0// LOG_TEX_ARR_CHANNEL_FLIPS
        for (int i = 0; i < 16*16; i++)
        {
            UE_LOG(LogTemp, Log, TEXT("px:%d; %d %d %d %d --- %d %d %d %d"),
                i,
                ((uint8*)pSourceMipData)[i * 4 + 0],
                ((uint8*)pSourceMipData)[i * 4 + 1],
                ((uint8*)pSourceMipData)[i * 4 + 2],
                ((uint8*)pSourceMipData)[i * 4 + 3],
                ((uint8*)DestSliceMipDataPtr0],
                ((uint8*)DestSliceMipDataPtr1],
                ((uint8*)DestSliceMipDataPtr2],
                ((uint8*)DestSliceMipDataPtr)[i * 4 + 3])
        }
#endif /* LOG_TEX_ARR_CHANNEL_FLIPS */

        // Unlock source texture
        if (CurrentSlicePtr->GetPlatformData()->Mips[0].BulkData.IsLocked())
        {
            CurrentSlicePtr->GetPlatformData()->Mips[0].BulkData.Unlock();
        }

        // Increase memory offset for writing
        CurrentTexArrMemoryOffset += SliceSize;
    }
    TexArr->GetPlatformData()->Mips[0].BulkData.Unlock();

    TexArr->UpdateResource();

#if 1
    int TextureIndex = 0;
    constexpr int StoneVoxelIdx = 2; /* two core voxels */
    constexpr int DirtVoxelIdx = 3;
    constexpr int GrassVoxelIdx = 4;

    // TextureArray->SourceTextures.Add(JAFGInstance->Stone);
    // VoxelSubsystem->VoxelMasks[StoneVoxelIdx].AddTextureIndex(ENormalLookup::Default, 0);

    // TextureArray->SourceTextures.Add(JAFGInstance->Dirt);
    // VoxelSubsystem->VoxelMasks[DirtVoxelIdx].AddTextureIndex(ENormalLookup::Default, 1);

    // VoxelSubsystem->VoxelMasks[GrassVoxelIdx].AddTextureIndex(ENormalLookup::Bottom, 1);
    // TextureArray->SourceTextures.Add(JAFGInstance->GrassTop);
    // VoxelSubsystem->VoxelMasks[GrassVoxelIdx].AddTextureIndex(ENormalLookup::Top, 2);
    // TextureArray->SourceTextures.Add(JAFGInstance->Grass);
    // VoxelSubsystem->VoxelMasks[GrassVoxelIdx].AddTextureIndex(ENormalLookup::Default, 3);

    // https://forums.unrealengine.com/t/cant-make-a-a-texture-array-at-runtime/157889/2

    // TextureArray->UpdateResource();
    // TextureArray->UpdateSourceFromSourceTextures();
#endif

    this->MDynamicOpaque           = UMaterialInstanceDynamic::Create(MaterialSettings->MOpaque.LoadSynchronous(), this);
    this->MDynamicFullBlendOpaque  = UMaterialInstanceDynamic::Create(MaterialSettings->MFullBlendOpaque.LoadSynchronous(), this);
    this->MDynamicFloraBlendOpaque = UMaterialInstanceDynamic::Create(MaterialSettings->MFloraBlendOpaque.LoadSynchronous(), this);

    this->MDynamicOpaque->SetTextureParameterValue("TexArr", TexArr);
    this->MDynamicFullBlendOpaque->SetTextureParameterValue("TexArr", TexArr);
    this->MDynamicFloraBlendOpaque->SetTextureParameterValue("TexArr", TexArr);

    // UE_LOG(LogTemp, Log, TEXT("UMaterialSubsystem::InitializeMaterials: Texture array size: %d"), TextureArray->SourceTextures.Num());

    return;
}
