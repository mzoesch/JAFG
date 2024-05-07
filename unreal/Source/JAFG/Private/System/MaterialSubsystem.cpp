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

    this->MDynamicGroups.Empty();

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
     * This is maybe a bit unnecessary. Instead of an array, we could just create different materials for each blend.
     * As a blend cannot be changed individually per voxel-face but only for the whole array instance.
     * But if we use normal UTexture2Ds for the blends, these blends get a lot blurry. This is not what we want as
     * we want to have sharp transitions between the different textures.
     * If we can fix this, we should use this approach instead.
     */
    UTexture2DArray* BlendTexArr = UTexture2DArray::CreateTransient(
        UMaterialSubsystem::TexArrWidthHorizontal,
        UMaterialSubsystem::TexArrWidthVertical,
        /*
         * No need to add here additional spaces (e.g., opaque)
         * as they will use a different arr instance as a parent.
         */
        this->Blends.Num(),
        /* The old pixel format. Look at Tex Channel flips for more information. */
        /* EPixelFormat::PF_R8G8B8A8, */
        EPixelFormat::PF_B8G8R8A8
    );
    BlendTexArr->Filter              = TextureFilter::TF_Nearest;
    BlendTexArr->SRGB                = true;
    BlendTexArr->CompressionSettings = TextureCompressionSettings::TC_Default;

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

    // Texture Texture-Array
    //////////////////////////////////////////////////////////////////////////
    void* TexArrMipDataPtr          = TexArr->GetPlatformData()->Mips[0].BulkData.Lock( LOCK_READ_WRITE );
    int64 CurrentTexArrMemoryOffset = 0;

    for (int32 SourceTexIndex = 0; SourceTexIndex < NamespaceTexCount; ++SourceTexIndex)
    {
        const FString& CurrentTexName    = TextureSubsystem->GetWorldTexture2DNameByIndex("JAFG", SourceTexIndex);
        /* Of type: Name, <Top>, <Flora> */
        TArray<FString> TexNameParts = TextureSubsystem->SplitTextureName(CurrentTexName);
        check( TexNameParts.Num() > 0 && TexNameParts.Num() < 4 )

        FString CurrentVoxelName = TexNameParts[0];
        const voxel_t  CurrentVoxelIndex = VoxelSubsystem->GetVoxelIndex("JAFG", CurrentVoxelName);
        if (CurrentVoxelIndex == ECommonVoxels::Null)
        {
            LOG_FATAL(LogMaterialSubsystem, "Voxel index is invalid for voxel [%s::%s]." , *FString("JAFG"), *CurrentVoxelName)
            return;
        }

        // We have something like StoneVoxel
        if (TexNameParts.Num() == 1)
        {
            VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddTextureGroup(ENormalLookup::Default, ETextureGroup::Opaque);
            VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddTextureIndex(ENormalLookup::Default, SourceTexIndex);
        }

        // We have something like GrassVoxel_Top_Full
        else if (TexNameParts.Num() == 3)
        {
            const FString NormalAsText = TexNameParts[1];
            const FString BlendAsText  = TexNameParts[2];
            check( NormalAsText.IsEmpty() == false )
            check( BlendAsText.IsEmpty() == false )

            const int32 Blend = this->Blends.Find(BlendAsText);
            check( Blend != this->NoBlend )

            if (ENormalLookup::IsValid(NormalAsText) == false)
            {
                LOG_FATAL(LogMaterialSubsystem, "Invalid normal lookup for voxel [%s::%s]. Faulty texture: %s.", *FString("JAFG"), *CurrentVoxelName, *CurrentTexName)
                return;
            }

            VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddSafeTextureGroup(ENormalLookup::FromString(NormalAsText), ETextureGroup::FromBlendArrIdx(Blend));
            VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddSafeTextureIndex(ENormalLookup::FromString(NormalAsText), SourceTexIndex);
        }

        else if (TexNameParts.Num() == 2)
        {
            // We have something like GrassVoxel_Flora
            if (this->Blends.Contains(TexNameParts[1]))
            {
                const int32 Blend = this->Blends.Find(TexNameParts[1]);
                check( Blend != this->NoBlend )
                VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddTextureGroup(ENormalLookup::Default, ETextureGroup::FromBlendArrIdx(Blend));
                VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddTextureIndex(ENormalLookup::Default, SourceTexIndex);
            }

            // We have something like GrassVoxel_Top
            else if (ENormalLookup::IsValid(TexNameParts[1]))
            {
                VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddSafeTextureGroup(ENormalLookup::FromString(TexNameParts[1]), ETextureGroup::Opaque);
                VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddSafeTextureIndex(ENormalLookup::FromString(TexNameParts[1]), SourceTexIndex);
            }
            else
            {
                LOG_FATAL(LogMaterialSubsystem, "Invalid normal lookup for voxel [%s::%s]. Faulty texture: %s.", *FString("JAFG"), *CurrentVoxelName, *CurrentTexName)
                return;
            }
        }
        else
        {
            checkNoEntry()
            LOG_FATAL(LogMaterialSubsystem, "Invalid texture name parts for voxel [%s::%s]. Faulty texture: %s.", *FString("JAFG"), *CurrentVoxelName, *CurrentTexName)
            return;
        }

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

        CurrentTexArrMemoryOffset += SliceSize;

        continue;
    }
    TexArr->GetPlatformData()->Mips[0].BulkData.Unlock();

    TexArr->UpdateResource();

    // Blend Texture-Array
    //////////////////////////////////////////////////////////////////////////
    check( this->Blends.Num() > 0 )

    void* BlendTexArrMipDataPtr          = BlendTexArr->GetPlatformData()->Mips[0].BulkData.Lock( LOCK_READ_WRITE );
    int64 CurrentBlendTexArrMemoryOffset = 0;

    for (int i = 0; i < this->Blends.Num(); ++i)
    {
        void* DestSliceMipDataPtr = static_cast<uint8*>(BlendTexArrMipDataPtr) + CurrentBlendTexArrMemoryOffset;

        UTexture2D* CurrentSlicePtr = TextureSubsystem->GetSafeBlendTexture2D(this->Blends[i]);
        if (CurrentSlicePtr == nullptr)
        {
            LOG_FATAL(LogMaterialSubsystem, "Missing source texture while making blend texture array, element: %d. For %s.", i, *this->Blends[i])
            return;
        }

        const void* CurrentSliceMipDataPtr = CurrentSlicePtr->GetPlatformMips()[0].BulkData.LockReadOnly();
        if (CurrentSliceMipDataPtr == nullptr)
        {
            LOG_FATAL(LogMaterialSubsystem, "Failed to aquire read only lock for source texture while making blend texture array, element: %d. For %s.", i, *this->Blends[i])
            return;
        }

        FMemory::Memcpy( DestSliceMipDataPtr, CurrentSliceMipDataPtr, SliceSize );

        if (CurrentSlicePtr->GetPlatformData()->Mips[0].BulkData.IsLocked())
        {
            CurrentSlicePtr->GetPlatformData()->Mips[0].BulkData.Unlock();
        }

        CurrentBlendTexArrMemoryOffset += SliceSize;

        continue;
    }

    // UTexture2D* CurrentAlphaSlice = TextureSubsystem->GetSafeBlendTexture2D("Flora");
    // const void* CurrentAlphaSliceMipDataPtr = CurrentAlphaSlice->GetPlatformMips()[0].BulkData.LockReadOnly();
    // FMemory::Memcpy( BlendTexArrMipDataPtr, CurrentAlphaSliceMipDataPtr, SliceSize );
    // if (CurrentAlphaSlice->GetPlatformData()->Mips[0].BulkData.IsLocked())
    // {
    //     CurrentAlphaSlice->GetPlatformData()->Mips[0].BulkData.Unlock();
    // }

    BlendTexArr->GetPlatformData()->Mips[0].BulkData.Unlock();

    BlendTexArr->UpdateResource();

    // Apply texture arrays to materials
    //////////////////////////////////////////////////////////////////////////
    check( this->MDynamicGroups.IsEmpty() )

    this->MDynamicGroups.Add(UMaterialInstanceDynamic::Create(MaterialSettings->MOpaque.LoadSynchronous(), this));
    this->MDynamicGroups[0]->SetTextureParameterValue("TexArr", TexArr);

    for (int i = 1; i < this->Blends.Num() + 1; ++i)
    {
        this->MDynamicGroups.Add(UMaterialInstanceDynamic::Create(MaterialSettings->MOpaqueBlend.LoadSynchronous(), this));

        this->MDynamicGroups[i]->SetTextureParameterValue("TexArr", TexArr);

        this->MDynamicGroups[i]->SetScalarParameterValue("BlendGroup", i - 1);
        this->MDynamicGroups[i]->SetTextureParameterValue("BlendTexArr", BlendTexArr);

        continue;
    }

    return;
}
