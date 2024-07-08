// Copyright 2024 mzoesch. All rights reserved.

#include "FrontEnd/GenPrevWorldSubsystem.h"

#include "ImageUtils.h"
#include "LocalSessionSupervisorSubsystem.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "System/TextureSubsystem.h"
#include "System/VoxelSubsystem.h"
#include "WorldCore/CurrentWorldInformationSubsystem.h"
#include "WorldCore/Entity/Cuboid.h"

AGenPrevAssets::AGenPrevAssets(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = true;

    this->SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
    this->SetRootComponent(this->SceneComponent);

    this->CaptureComponent = ObjectInitializer.CreateDefaultSubobject<USceneCaptureComponent2D>(this, TEXT("CaptureComponent"));
    this->CaptureComponent->SetupAttachment(this->SceneComponent);
    this->CaptureComponent->bCaptureEveryFrame           = false;
    this->CaptureComponent->bCaptureOnMovement           = false;
    this->CaptureComponent->bAlwaysPersistRenderingState = true;
    this->CaptureComponent->FOVAngle                     = 30.0f;
    this->CaptureComponent->CaptureSource                = ESceneCaptureSource::SCS_FinalColorLDR;
    /* TODO We may want to use this later to save on memory and performance. But this works for now. */
    /* this->CaptureComponent->PrimitiveRenderMode        = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList; */
    this->CaptureComponent->PrimitiveRenderMode           = ESceneCapturePrimitiveRenderMode::PRM_LegacySceneCapture;

    this->CurrentIndex = 0;

    return;
}

void AGenPrevAssets::BeginPlay()
{
    Super::BeginPlay();

    this->GenPrevWorldSubsystem = this->GetWorld()->GetSubsystem<UGenPrevWorldSubsystem>();   jcheck( this->GenPrevWorldSubsystem )
    this->VoxelSubsystem        = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();   jcheck( this->VoxelSubsystem )
    this->TextureSubsystem      = this->GetGameInstance()->GetSubsystem<UTextureSubsystem>(); jcheck( this->TextureSubsystem )

    this->CurrentIndex = this->VoxelSubsystem->GetCommonVoxelNum(); jcheck( this->CurrentIndex > 0 )

    return;
}

void AGenPrevAssets::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (this->CurrentIndex > this->VoxelSubsystem->GetVoxelNum() - 1)
    {
        this->SetActorTickEnabled(false);

        LOG_VERBOSE(LogGenPrevAssets, "Finished generating voxel assets at [%d]. Forwarding event to owning subsystem.", this->CurrentIndex)

        this->GenPrevWorldSubsystem->OnFinishedGeneratingAssetsDelegate.Execute();

        return;
    }

    ACuboid* Cuboid = this->GetWorld()->SpawnActor<ACuboid>(ACuboid::StaticClass(), this->PrevTransform, FActorSpawnParameters());
    Cuboid->SetCuboidVoxelDimensions(100.0f);
    Cuboid->GenerateMesh(this->CurrentIndex);

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

    FImage Image = FImage();
    FImageUtils::GetRenderTargetImage(this->CaptureComponent->TextureTarget, Image);

    UTexture2D* Tex      = FImageUtils::CreateTexture2DFromImage(Image);
    FImage      OutImage = FImage();
    OutImage.SizeX       = Image.SizeX;
    OutImage.SizeY       = Image.SizeY;
    OutImage.NumSlices   = Image.NumSlices;
    OutImage.Format      = Image.Format; /* Currently only ERawImageFormat::Type::BGRA8 supported. */
    OutImage.GammaSpace  = Image.GammaSpace;
    jcheck( OutImage.IsImageInfoValid() )
    const int64 NumPixels     = (int64) (OutImage.SizeX * OutImage.SizeY * OutImage.NumSlices);
    const int64 BytesPerPixel = AGenPrevAssets::GetBytesPerPixel(OutImage.Format);
    const int64 NumBytes      = NumPixels * BytesPerPixel;
    OutImage.RawData.Empty(NumBytes);
    OutImage.RawData.AddUninitialized(NumBytes);
    FTexture2DMipMap* Mip = &Tex->GetPlatformData()->Mips[0];
    const uint32 Width    = Mip->SizeX;
    const uint32 Height   = Mip->SizeY;
    FByteBulkData* Bulk   = &Mip->BulkData;
    const FColor* Data    = static_cast<FColor*>(Bulk->Lock(EBulkDataLockFlags::LOCK_READ_ONLY));

    LOG_VERBOSE(
        LogGenPrevAssets,
        "Generating preview texture for %s [%d/%d] (x:%d-y:%d) with raw data size: %lld of format %d.",
        *this->VoxelSubsystem->GetVoxelName(this->CurrentIndex),
        this->CurrentIndex, this->VoxelSubsystem->GetVoxelNum(),
        OutImage.SizeX, OutImage.SizeY,
        OutImage.RawData.Num(),
        OutImage.Format
    )

#if WITH_EDITOR
    if (Image.Format != ERawImageFormat::Type::BGRA8)
    {
        LOG_FATAL(LogGenPrevAssets, "Unsupported raw image format %d.", Image.Format)
        return;
    }
#endif /* WITH_EDITOR */

    int RawDataCursor = 0;
    for (uint32 h = 0; h < Height; ++h) { for (uint32 w = 0; w < Width; ++w)
    {
        FColor Pixel = Data[w + h * Width];

        if (
                   Pixel.A < 255
            ||  (
                   Pixel.R < AGenPrevAssets::NonAlphaThreshold
                && Pixel.G < AGenPrevAssets::NonAlphaThreshold
                && Pixel.B < AGenPrevAssets::NonAlphaThreshold
                )
            )
        {
            Pixel = FColor::Transparent;
        }

        jcheck( static_cast<int64>(w) < OutImage.SizeX && static_cast<int64>(h) < OutImage.SizeY )

        //
        // We may want to manipulate the raw data binary directly. Instead of using the RawData array.
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
    } }

    Bulk->Unlock();
    Tex->UpdateResource();

    const FString FileName = FString::Printf(TEXT("%s.png"), *this->VoxelSubsystem->GetVoxelName(this->CurrentIndex));
    const FString OutPath  = this->TextureSubsystem->GeneratedAssetsDirectoryRelative / FileName;

    FImageUtils::SaveImageByExtension(*OutPath, OutImage);

    Cuboid->Destroy();

    ++this->CurrentIndex;

    return;
}

int64 AGenPrevAssets::GetBytesPerPixel(const ERawImageFormat::Type Format)
{
    /*
     * Shamelessly copied from IMAGECORE_API ERawImageFormat::GetBytesPerPixel(ERawImageFormat::Type).
     * We do not want to include the entire module just for this one function.
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
        jcheckNoEntry();
        break;
    }
    }

    return OutBytesPerPixel;
}

UGenPrevWorldSubsystem::UGenPrevWorldSubsystem(void) : Super()
{
    return;
}

void UGenPrevWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

bool UGenPrevWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return Outer->GetWorld()->GetName() == RegisteredWorlds::GenPrevAssets;
}

void UGenPrevWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    this->BindToOnFinishedGeneratingAssetsDelegate();

    this->GetWorld()->SpawnActor<AGenPrevAssets>(AGenPrevAssets::StaticClass(), FTransform::Identity);

    return;
}

void UGenPrevWorldSubsystem::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void UGenPrevWorldSubsystem::BindToOnFinishedGeneratingAssetsDelegate(void)
{
    this->OnFinishedGeneratingAssetsDelegate.BindLambda([this] (void) -> void
    {
        if (
            this->GetWorld()->GetGameInstance()->GetSubsystem<UCurrentWorldInformationSubsystem>()
                ->TravelClientAroundWhileNotInAnySession(ERegisteredWorlds::FrontEnd) == false
        )
        {
            LOG_FATAL(LogGenPrevAssets, "Failed to travel client to Front End level.")
        }

        return;
    });

    return;
}
