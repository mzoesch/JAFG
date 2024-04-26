// Copyright 2024 mzoesch. All rights reserved.

#include "GenPrevAssets/GenPrevAssetsComponent.h"

#include "ImageUtils.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Misc/CommonLogging.h"
#include "Network/NetworkStatics.h"
#include "System/TextureSubsystem.h"
#include "World/Entity/Cuboid.h"
#include "World/Voxel/VoxelSubsystem.h"

AGenPrevAssetsActor::AGenPrevAssetsActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = true;

    this->SceneComponent = this->CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    this->SetRootComponent(this->SceneComponent);

    this->CaptureComponent = this->CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera"));
    this->CaptureComponent->bCaptureEveryFrame = false;
    this->CaptureComponent->bCaptureOnMovement = false;
    this->CaptureComponent->bAlwaysPersistRenderingState = true;
    this->CaptureComponent->FOVAngle = 30.0f;
    this->CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    /* We may want to use this later to save on memory and performance. But this works for now. */
    /* this->CaptureComponent->PrimitiveRenderMode          = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList; */
    this->CaptureComponent->PrimitiveRenderMode             = ESceneCapturePrimitiveRenderMode::PRM_LegacySceneCapture;
    this->CaptureComponent->SetupAttachment(this->SceneComponent);

    this->CurrentVoxelIndex = 0;

    this->OnGenPrevAssetsCompleteEvent.Unbind();

    return;
}

void AGenPrevAssetsActor::BeginPlay(void)
{
    Super::BeginPlay();

    if ((UNetworkStatics::IsSafeStandalone(this) || UNetworkStatics::IsSafeClient(this)) == false)
    {
        LOG_FATAL(LogTemp, "AGenPrevAssetsActor::BeginPlay: This actor is not safe to run on the server.")
        return;
    }

    check( this->GetGameInstance() )
    check( this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>() )
    this->CurrentVoxelIndex = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>()->GetCommonVoxelNum();
    check( this->CurrentVoxelIndex > 0 )
    this->VoxelNum = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>()->GetVoxelNum();
    check( this->CurrentVoxelIndex <= this->VoxelNum )

    return;
}

void AGenPrevAssetsActor::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (this->CurrentVoxelIndex > this->VoxelNum - 1)
    {
        this->SetActorTickEnabled(false);

        LOG_VERBOSE(LogGenPrevAssets, "Finished generating preview textures. Broadcasting.")

        if (this->OnGenPrevAssetsCompleteEvent.ExecuteIfBound() == false)
        {
#if WITH_EDITOR
            LOG_WARNING(LogGenPrevAssets, "Failed to execute bound event. Intentional?")
#else /* WITH_EDITOR */
            LOG_FATAL(LogGenPrevAssets, "Failed to execute bound event.")
#endif /* !WITH_EDITOR */
        }

        return;
    }

    this->GenerateForCurrentPrevAsset();

    this->CurrentVoxelIndex++;

    return;
}

void AGenPrevAssetsActor::GenerateForCurrentPrevAsset(void)
{
    check( this->GetGameInstance() )
    const UVoxelSubsystem* VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    check( VoxelSubsystem )

    /* Spawn cuboid. */

    check( this->GetWorld() )
    ACuboid* Cuboid = this->GetWorld()->SpawnActor<ACuboid>(ACuboid::StaticClass(), AGenPrevAssetsActor::PrevTransform, FActorSpawnParameters());
    check( Cuboid )
    Cuboid->SetAccumulatedIndex(this->CurrentVoxelIndex);
    Cuboid->RegenerateProceduralMesh();

    /* Image capture. */

    this->CaptureComponent->TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(
        this,
        1024,
        1024,
        ETextureRenderTargetFormat::RTF_RGBA8,
        FLinearColor::Black,
        false
    );
    this->CaptureComponent->TextureTarget->TargetGamma = 1.8f;
    this->CaptureComponent->CaptureScene();

    FImage              CapturedImage;
    FImageUtils::GetRenderTargetImage(this->CaptureComponent->TextureTarget, CapturedImage);

    UTexture2D*         Tex             = FImageUtils::CreateTexture2DFromImage(CapturedImage);
    FImage              OutImage        = FImage();
    OutImage.SizeX                      = CapturedImage.SizeX;
    OutImage.SizeY                      = CapturedImage.SizeY;
    OutImage.NumSlices                  = CapturedImage.NumSlices;
    OutImage.Format                     = CapturedImage.Format; /* Currently only ERawImageFormat::Type::BGRA8 supported. */
    OutImage.GammaSpace                 = CapturedImage.GammaSpace;
    check( OutImage.IsImageInfoValid() )
    const int64         NumPixels       = (int64) (OutImage.SizeX * OutImage.SizeY * OutImage.NumSlices);
    const int64         BytesPerPixel   = AGenPrevAssetsActor::GetBytesPerPixel(OutImage.Format);
    const int64         NumBytes        = NumPixels * BytesPerPixel;
    OutImage.RawData.Empty(NumBytes);
    OutImage.RawData.AddUninitialized(NumBytes);
    // TODO Is this important???
    // Tex->MipGenSettings                 = TextureMipGenSettings::TMGS_NoMipmaps;
    FTexture2DMipMap*   Mip             = &Tex->GetPlatformData()->Mips[0];
    const uint32        Width           = Mip->SizeX;
    const uint32        Height          = Mip->SizeY;
    FByteBulkData*      Bulk            = &Mip->BulkData;
    const FColor*       Data            = static_cast<FColor*>(Bulk->Lock(EBulkDataLockFlags::LOCK_READ_ONLY));

    LOG_DISPLAY(
        LogGenPrevAssets,
        "Generating Generating preview texture for %s [%d/%d] (x:%d-y:%d) with raw data size: %lld of format %d.",
        *VoxelSubsystem->GetVoxelName(this->CurrentVoxelIndex), this->CurrentVoxelIndex, this->VoxelNum, OutImage.SizeX, OutImage.SizeY, OutImage.RawData.Num(), OutImage.Format
    )

#if WITH_EDITOR
    if (CapturedImage.Format != ERawImageFormat::BGRA8)
    {
        LOG_FATAL(LogGenPrevAssets, "Raw image format %d is not supported.", CapturedImage.Format)
        return;
    }
#endif /* WITH_EDITOR */

    /* Low detail removal. */

    int RawDataCursor = 0;
    for (uint32 h = 0; h < Height; ++h) { for (uint32 w = 0; w < Width; ++w)
    {
        FColor Pixel = Data[w + h * Width];

        if (
               Pixel.A < 255
            || (
                       Pixel.R < AGenPrevAssetsActor::NonAlphaThreshold
                    && Pixel.G < AGenPrevAssetsActor::NonAlphaThreshold
                    && Pixel.B < AGenPrevAssetsActor::NonAlphaThreshold
               )
            )
        {
            Pixel = FColor::Transparent;
        }

        /* Why does the normal check cause a signed / unsigned mismatch? */
        checkSlow( w < OutImage.SizeX && h < OutImage.SizeY );

        //
        // We may want to manipulate the raw data binary directly. Instead of using the Raw Data array.
        // We can use the FColor::ToPackedBGRA() to get the raw memory bits.
        //
        // int64    Offset   = 0;
        // Offset           += w;
        // Offset           += h * OutImage.SizeX;
        // uint8*   RawPtr   = ( uint8* ) & OutImage.RawData;
        // RawPtr           += Offset;
        //

        OutImage.RawData[RawDataCursor++] = Pixel.B; /* Blue  Channel */
        OutImage.RawData[RawDataCursor++] = Pixel.G; /* Green Channel */
        OutImage.RawData[RawDataCursor++] = Pixel.R; /* Red   Channel */
        OutImage.RawData[RawDataCursor++] = Pixel.A; /* Alpha Channel */

        continue;
    }}

    Bulk->Unlock();
    Tex->UpdateResource();

    /* Save to extern. */

    const FString FileName = FString::Printf(TEXT("%s.png"), *VoxelSubsystem->GetVoxelName(this->CurrentVoxelIndex));
    const FString OutPath  = UTextureSubsystem::GeneratedAssetsDirectoryRelative / FileName;
    FImageUtils::SaveImageByExtension(*OutPath, OutImage);

    /* Cleanup. */

    if (Cuboid->Destroy() == false)
    {
        LOG_FATAL(LogGenPrevAssets, "Failed to destroy cuboid for voxel: %d.", this->CurrentVoxelIndex)
    }

    return;
}

int64 AGenPrevAssetsActor::GetBytesPerPixel(const ERawImageFormat::Type Format)
{
    /*
     * Shamelessly copied from IMAGECORE_API ERawImageFormat::GetBytesPerPixel(ERawImageFormat::Type).
     *
     * This method is rewritten because of a linker error. If this can be resolved, the original method should be used.
     *
     * error LNK2019: unresolved external symbol
     *      "__declspec(dllimport) public: void __cdecl FImage::Init(int,int,enum ERawImageFormat::Type,enum EGammaSpace)"
     *      (__imp_?Init@FImage@@QEAAXHHW4Type@ERawImageFormat@@W4EGammaSpace@@@Z) referenced in function "public:
     *      virtual void __cdecl AGenPrevAssetsActor::GenerateForCurrentPrevAsset(void)" (?GenerateForCurrentPrevAsset@AGenPrevAssetsActor@@UEAAXM@Z)
     *      [...]\JAFG\unreal\Binaries\Win64\UnrealEditor-JAFG.patch_YXZ.exe : fatal error LNK1120: 1 unresolved externals
     *
     */

    int64 OutBytesPerPixel = 0;

    switch (Format)
    {

    case ERawImageFormat::G8:
    {
        OutBytesPerPixel = 1;
        break;
    }

    case ERawImageFormat::G16:
    case ERawImageFormat::R16F:
    {
        OutBytesPerPixel = 2;
        break;
    }

    case ERawImageFormat::R32F:
    case ERawImageFormat::BGRA8:
    case ERawImageFormat::BGRE8:
    {
        OutBytesPerPixel = 4;
        break;
    }

    case ERawImageFormat::RGBA16:
    case ERawImageFormat::RGBA16F:
    {
        OutBytesPerPixel = 8;
        break;
    }

    case ERawImageFormat::RGBA32F:
    {
        OutBytesPerPixel = 16;
        break;
    }

    default:
    {
        LOG_FATAL(LogGenPrevAssets, "Unsupported raw image format %d.", Format)
        break;
    }

    }

    return OutBytesPerPixel;
}

UGenPrevAssetsComponent::UGenPrevAssetsComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = false;
}

void UGenPrevAssetsComponent::BeginPlay(void)
{
    Super::BeginPlay();

    check( this->GetWorld() )
    this->GenPrevAssetsActor = this->GetWorld()->SpawnActor<AGenPrevAssetsActor>(AGenPrevAssetsActor::StaticClass(), FTransform::Identity, FActorSpawnParameters());
    check( GenPrevAssetsActor )

    if (this->GenPrevAssetsActor->OnGenPrevAssetsCompleteEvent.IsBound())
    {
        LOG_FATAL(LogGenPrevAssets, "OnGenPrevAssetsCompleteEvent is already bound.")
        return;
    }

    this->GenPrevAssetsActor->OnGenPrevAssetsCompleteEvent.BindLambda( [this] (void) -> void
    {
        LOG_DISPLAY(LogGenPrevAssets, "Finished generating preview textures. Other processes may now continue.")

        /*
         * Later on here than load the front end. If shipping ...
         */

        return;
    });

    return;
}
