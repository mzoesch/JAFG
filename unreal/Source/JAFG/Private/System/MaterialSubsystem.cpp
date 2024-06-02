// Copyright 2024 mzoesch. All rights reserved.

#include "System/MaterialSubsystem.h"

#include "JAFGMaterialSettings.h"
#include "Engine/Texture2DArray.h"
#include "System/JAFGGameInstance.h"
#include "System/VoxelSubsystem.h"
#include "System/TextureSubsystem.h"

/**
 * If true, the R and G channel of the texture array will be flipped with each other.
 * As the source texture from the disk may have the wrong channel order, we have to fix this here while memcpy-ing the
 * bulk data of the source textures.
 *
 * This was a problem, but is currently not required on WIN64. But let here for future reference and some platforms
 * may require this in the future.
 */
#define PERFORM_R_AND_B_CHANNEL_FLIP 0

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
    this->InitializeDestructionMaterial();

    return;
}

void UMaterialSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
}

void UMaterialSubsystem::InitializeMaterials(void)
{
    const UJAFGGameInstance*     MyGameInstance   = Cast<UJAFGGameInstance>(this->GetWorld()->GetGameInstance());

    if (MyGameInstance == nullptr)
    {
        LOG_FATAL(LogMaterialSubsystem, "Game instance is not of type UJAFGGameInstance.")
        return;
    }

    UVoxelSubsystem*             VoxelSubsystem   = MyGameInstance->GetSubsystem<UVoxelSubsystem>();
    UTextureSubsystem*           TextureSubsystem = MyGameInstance->GetSubsystem<UTextureSubsystem>();
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

    // Load alpha masks
    //////////////////////////////////////////////////////////////////////////
    this->Blends = TextureSubsystem->LoadAllBlendTextureNames();
#if !UE_BUILD_SHIPPING
    if (this->Blends.Num() == 0)
    {
        LOG_ERROR(LogMaterialSubsystem, "No blend textures found. Was this intentional?")
    }
#endif /* !UE_BUILD_SHIPPING */

    // Create the dynamically generated materials
    //////////////////////////////////////////////////////////////////////////
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
    if (TexArrMipDataPtr == nullptr)
    {
        LOG_FATAL(LogMaterialSubsystem, "Failed to acquire read write lock for texture array.")
        return;
    }
    int64 CurrentTexArrMemoryOffset = 0;

    for (int32 SourceTexIndex = 0; SourceTexIndex < NamespaceTexCount; ++SourceTexIndex)
    {
        const FString& CurrentTexName    = TextureSubsystem->GetWorldTexture2DNameByIndex("JAFG", SourceTexIndex);

        /*
         * TexNameParts is of type: VoxelName, <VoxelNormal>, <VoxelBlend> (While <> means optional)
         * See this method's documentation for more information.
         */
        const TArray<FString> CurrentTexSplits = TextureSubsystem->SplitTextureName(CurrentTexName);
        check( CurrentTexSplits.Num() > 0 && CurrentTexSplits.Num() < 4 )

        const FString CurrentVoxelName = CurrentTexSplits[0];

        const voxel_t CurrentVoxelIndex = VoxelSubsystem->GetVoxelIndex("JAFG", CurrentVoxelName);
        if (CurrentVoxelIndex == ECommonVoxels::Null)
        {
            LOG_FATAL(LogMaterialSubsystem, "Voxel index is invalid for voxel [%s::%s]." , *FString("JAFG"), *CurrentVoxelName)
            return;
        }

        /* If true, we have something like: StoneVoxel. */
        if (CurrentTexSplits.Num() == 1)
        {
            VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddTextureGroup(ENormalLookup::Default, ETextureGroup::Opaque);
            VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddTextureIndex(ENormalLookup::Default, SourceTexIndex);
        }

        /* If true, we have something like: GrassVoxel_Top_Full. */
        else if (CurrentTexSplits.Num() == 3)
        {
            const FString NormalAsText = CurrentTexSplits[1];            check( NormalAsText.IsEmpty() == false )
            const FString BlendAsText  = CurrentTexSplits[2];            check( BlendAsText.IsEmpty()  == false )

            const int32   Blend        = this->Blends.Find(BlendAsText); check( Blend != this->NoBlend )

            if (ENormalLookup::IsValid(NormalAsText) == false)
            {
                LOG_FATAL(
                    LogMaterialSubsystem,
                    "Invalid normal lookup for voxel [%s::%s]. Faulty texture: %s.",
                    *FString("JAFG"), *CurrentVoxelName, *CurrentTexName
                )
                return;
            }

            VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddSafeTextureGroup(ENormalLookup::FromString(NormalAsText), ETextureGroup::FromBlendArrIdx(Blend));
            VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddSafeTextureIndex(ENormalLookup::FromString(NormalAsText), SourceTexIndex);
        }

        /* If true, we have something like: MyVoxel_<SUFFIX>. */
        else if (CurrentTexSplits.Num() == 2)
        {
            /* If true, we have something like: GrassVoxel_Flora. */
            if (this->Blends.Contains(CurrentTexSplits[1]))
            {
                const int32 Blend = this->Blends.Find(CurrentTexSplits[1]); check( Blend != this->NoBlend )

                VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddTextureGroup(ENormalLookup::Default, ETextureGroup::FromBlendArrIdx(Blend));
                VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddTextureIndex(ENormalLookup::Default, SourceTexIndex);
            }

            /* If true, we have something like: GrassVoxel_Bot. */
            else if (ENormalLookup::IsValid(CurrentTexSplits[1]))
            {
                VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddSafeTextureGroup(ENormalLookup::FromString(CurrentTexSplits[1]), ETextureGroup::Opaque);
                VoxelSubsystem->VoxelMasks[CurrentVoxelIndex].AddSafeTextureIndex(ENormalLookup::FromString(CurrentTexSplits[1]), SourceTexIndex);
            }

            else
            {
                LOG_FATAL(
                    LogMaterialSubsystem,
                    "Invalid normal lookup for voxel [%s::%s]. Faulty texture: %s.",
                    *FString("JAFG"), *CurrentVoxelName, *CurrentTexName
                )
                return;
            }
        }

        else
        {
            LOG_FATAL(
                LogMaterialSubsystem,
                "Invalid texture name parts for voxel [%s::%s]. Faulty texture: %s.",
                *FString("JAFG"), *CurrentVoxelName, *CurrentTexName
            )
            return;
        }

        void* DestinationSliceMipDataPtr = static_cast<uint8*>(TexArrMipDataPtr) + CurrentTexArrMemoryOffset;

        UTexture2D* CurrentSlicePtr = TextureSubsystem->GetWorldTexture2D("JAFG", CurrentTexName);
        if (CurrentSlicePtr == nullptr)
        {
            LOG_FATAL(
                LogMaterialSubsystem,
                "Missing source texture while making texture array, element: %d. For %s::%s.",
                SourceTexIndex, *FString("JAFG"), *CurrentTexName
            )
            return;
        }

        const void* CurrentSliceMipDataPtr = CurrentSlicePtr->GetPlatformMips()[0].BulkData.LockReadOnly();
        if (CurrentSliceMipDataPtr == nullptr)
        {
            LOG_FATAL(
                LogMaterialSubsystem,
                "Failed to aquire read only lock for source texture while making texture array, element: %d. For %s::%s.",
                SourceTexIndex, *FString("JAFG"), *CurrentTexName
            )
            return;
        }

        FMemory::Memcpy(DestinationSliceMipDataPtr, CurrentSliceMipDataPtr, SliceSize);

#if PERFORM_R_AND_B_CHANNEL_FLIP
#define CHANNEL_COUNT 4 /* RGBA */

        //
        // Okay, so there is a small problem with the RGBA format.
        // The source texture is in B8G8R8A8 format, but the texture array is in R8G8B8A8 format.
        // So what we have to do is to flip the red and blue channels.
        // We do this directly during the memcpy operation. We do not want to modify the bulk data of the source
        // texture directly, because they are used elsewhere (mainly in the HUD and OSD) where the format is correct.
        //

        for (int Channel = 0; Channel < CHANNEL_COUNT; ++Channel)
        {
            for (
                int i = 0;
                i < UMaterialSubsystem::TexArrWidthHorizontal * UMaterialSubsystem::TexArrWidthVertical;
                ++i
            )
            {
                /*
                 * This actually works here (somehow) on my machine. But if this causes problems on other machines, we
                 * may want to use the FMemory::Memcpy function instead.
                 */
                static_cast<uint8*>( DestinationSliceMipDataPtr )[i * CHANNEL_COUNT + Channel]
                /*
                 * I really do not know why the IDEA is complaining here about the cast?
                 * We cannot cas this crap because it is a const void pointer.
                 */
                // ReSharper disable once CppCStyleCast
                    = ( (uint8*) CurrentSliceMipDataPtr )[i * CHANNEL_COUNT + (Channel == 0 ? 2 : Channel == 2 ? 0 : Channel)];
            }
        }
#if LOG_TEX_ARR_CHANNEL_FLIPS
        for (
            int i = 0;
            i < UMaterialSubsystem::TexArrWidthHorizontal * UMaterialSubsystem::TexArrWidthVertical;
            ++i
        )
        {
            LOG_VERBOSE(LogMaterialSubsystem, "Performed flip %d: SRC --- DET ; BGRA --- RGBA ; %d %d %d %d --- %d %d %d %d.",
                i,
                ( (uint8*) CurrentSliceMipDataPtr )             [i * CHANNEL_COUNT + 0],
                ( (uint8*) CurrentSliceMipDataPtr )             [i * CHANNEL_COUNT + 1],
                ( (uint8*) CurrentSliceMipDataPtr )             [i * CHANNEL_COUNT + 2],
                ( (uint8*) CurrentSliceMipDataPtr )             [i * CHANNEL_COUNT + 3],
                static_cast<uint8*>(DestinationSliceMipDataPtr) [i * CHANNEL_COUNT + 0],
                static_cast<uint8*>(DestinationSliceMipDataPtr) [i * CHANNEL_COUNT + 1],
                static_cast<uint8*>(DestinationSliceMipDataPtr) [i * CHANNEL_COUNT + 2],
                static_cast<uint8*>(DestinationSliceMipDataPtr) [i * CHANNEL_COUNT + 3]
            )
        }
#endif /* LOG_TEX_ARR_CHANNEL_FLIPS */

#undef CHANNEL_COUNT
#endif /* PERFORM_R_AND_B_CHANNEL_FLIP */

        /*
         * Sometimes the bulk data is automatically unlocked. Probably the compiler or the engine does some
         * shenanigans-magic here (but not always, so that's eccentric).
         * But unlocking an unlocked bulk will result in a crash. That's why we have to check here about the lockiness.
         */
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
    if (BlendTexArrMipDataPtr == nullptr)
    {
        LOG_FATAL(LogMaterialSubsystem, "Failed to acquire read write lock for blend texture array.")
        return;
    }
    int64 CurrentBlendTexArrMemoryOffset = 0;

    for (int i = 0; i < this->Blends.Num(); ++i)
    {
        void* DestSliceMipDataPtr = static_cast<uint8*>(BlendTexArrMipDataPtr) + CurrentBlendTexArrMemoryOffset;

        UTexture2D* CurrentSlicePtr = TextureSubsystem->GetSafeBlendTexture2D(this->Blends[i]);
        if (CurrentSlicePtr == nullptr)
        {
            LOG_FATAL(
                LogMaterialSubsystem,
                "Missing source texture while making blend texture array, element: %d. For %s.",
                i, *this->Blends[i]
            )
            return;
        }

        const void* CurrentSliceMipDataPtr = CurrentSlicePtr->GetPlatformMips()[0].BulkData.LockReadOnly();
        if (CurrentSliceMipDataPtr == nullptr)
        {
            LOG_FATAL(
                LogMaterialSubsystem,
                "Failed to aquire read only lock for source texture while making blend texture array, element: %d. For %s.",
                i, *this->Blends[i]
            )
            return;
        }

        FMemory::Memcpy( DestSliceMipDataPtr, CurrentSliceMipDataPtr, SliceSize );

        /* See the for-loop above for more information about this if block. */
        if (CurrentSlicePtr->GetPlatformData()->Mips[0].BulkData.IsLocked())
        {
            CurrentSlicePtr->GetPlatformData()->Mips[0].BulkData.Unlock();
        }

        CurrentBlendTexArrMemoryOffset += SliceSize;

        continue;
    }
    BlendTexArr->GetPlatformData()->Mips[0].BulkData.Unlock();

    BlendTexArr->UpdateResource();

    // Apply texture arrays to materials
    //////////////////////////////////////////////////////////////////////////
    jcheck( this->MDynamicGroups.IsEmpty() )

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

void UMaterialSubsystem::InitializeDestructionMaterial(void)
{
    const UJAFGGameInstance*     MyGameInstance   = Cast<UJAFGGameInstance>(this->GetWorld()->GetGameInstance());

    if (MyGameInstance == nullptr)
    {
        LOG_FATAL(LogMaterialSubsystem, "Game instance is not of type UJAFGGameInstance.")
        return;
    }

    UTextureSubsystem*           TextureSubsystem = MyGameInstance->GetSubsystem<UTextureSubsystem>();
    const UJAFGMaterialSettings* MaterialSettings = GetDefault<UJAFGMaterialSettings>();

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

    const TArray<FString> DestructionNames = TextureSubsystem->LoadAllDestructionTextureNames(TEXT("JAFG"));
    LOG_VERBOSE(LogMaterialSubsystem, "Using %d destruction textures found in namespace %s.", DestructionNames.Num(), *FString("JAFG"))

    // Create the dynamically generated material
    //////////////////////////////////////////////////////////////////////////
    UTexture2DArray* TexArr = UTexture2DArray::CreateTransient(
        UMaterialSubsystem::TexArrWidthHorizontal,
        UMaterialSubsystem::TexArrWidthVertical,
        DestructionNames.Num(),
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

    // Texture Texture-Array
    //////////////////////////////////////////////////////////////////////////
    void* TexArrMipDataPtr          = TexArr->GetPlatformData()->Mips[0].BulkData.Lock( LOCK_READ_WRITE );
    if (TexArrMipDataPtr == nullptr)
    {
        LOG_FATAL(LogMaterialSubsystem, "Failed to acquire read write lock for texture array.")
        return;
    }
    int64 CurrentTexArrMemoryOffset = 0;

    for (const FString& DestructionName : DestructionNames)
    {
        void* DestinationSliceMipDataPtr = static_cast<uint8*>(TexArrMipDataPtr) + CurrentTexArrMemoryOffset;

        UTexture2D* CurrentSlicePtr = TextureSubsystem->GetWorldDestructionTexture2D("JAFG", DestructionName);
        if (CurrentSlicePtr == nullptr)
        {
            LOG_FATAL(
                LogMaterialSubsystem,
                "Missing source texture while making destruction array. Faulty tex %s::%s.",
                *FString("JAFG"), *DestructionName
            )
            return;
        }

        const void* CurrentSliceMipDataPtr = CurrentSlicePtr->GetPlatformMips()[0].BulkData.LockReadOnly();
        if (CurrentSliceMipDataPtr == nullptr)
        {
            LOG_FATAL(
                LogMaterialSubsystem,
                "Failed to aquire read only lock for source texture while making destruction array. Faulty tex %s::%s.",
                *FString("JAFG"), *DestructionName
            )
            return;
        }

        FMemory::Memcpy(DestinationSliceMipDataPtr, CurrentSliceMipDataPtr, SliceSize);

#if PERFORM_R_AND_B_CHANNEL_FLIP
        LOG_FATAL(LogMaterialSubsystem, "R and B channel flip is not implemented for destruction textures.")
#endif /* PERFORM_R_AND_B_CHANNEL_FLIP */

        if (CurrentSlicePtr->GetPlatformData()->Mips[0].BulkData.IsLocked())
        {
            CurrentSlicePtr->GetPlatformData()->Mips[0].BulkData.Unlock();
        }

        CurrentTexArrMemoryOffset += SliceSize;

        continue;
    }
    TexArr->GetPlatformData()->Mips[0].BulkData.Unlock();

    TexArr->UpdateResource();

    // Apply destruction arrays to materials
    //////////////////////////////////////////////////////////////////////////
    this->MDynamicDestruction = UMaterialInstanceDynamic::Create(MaterialSettings->MDestruction.LoadSynchronous(), this);
    this->MDynamicDestruction->SetTextureParameterValue("TexArr", TexArr);

    return;
}

#undef PERFORM_R_AND_B_CHANNEL_FLIP
