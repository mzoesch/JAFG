// © 2023 mzoesch. All rights reserved.

#include "Assets/GenPrevAssets.h"

#include "ImageUtils.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "ImageCore.h"

#include "Assets/General.h"
#include "Lib/Cuboid.h"
#include "Core/GI_Master.h"

#define UIL_LOG(Verbosity, Format, ...)     UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define GAME_INSTANCE                       Cast<UGI_Master>(this->GetGameInstance())

AGenPrevAssets::AGenPrevAssets()
{
    this->PrimaryActorTick.bCanEverTick                     = true;

    this->SceneComponent                                    = this->CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    this->CaptureComponent                                  = this->CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera"));
    this->CaptureComponent->bCaptureEveryFrame              = false;
    this->CaptureComponent->bCaptureOnMovement              = false;
    this->CaptureComponent->bAlwaysPersistRenderingState    = true;
    this->CaptureComponent->FOVAngle                        = 30.0f;
    this->CaptureComponent->CaptureSource                   = ESceneCaptureSource::SCS_FinalColorLDR;
    /* TODO We may want to use this later to save on memory and performance. But this works for now. */
    /*this->CaptureComponent->PrimitiveRenderMode           = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList; */
    this->CaptureComponent->PrimitiveRenderMode             = ESceneCapturePrimitiveRenderMode::PRM_LegacySceneCapture;

    this->SetRootComponent(this->SceneComponent);
    this->CaptureComponent->SetupAttachment(this->SceneComponent);

    this->PrevIndex = 0;

    return;
}

void AGenPrevAssets::BeginPlay()
{
    Super::BeginPlay();
    this->PrevIndex = GAME_INSTANCE->GetCoreVoxelNum();
    return;
}

void AGenPrevAssets::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (this->PrevIndex > GAME_INSTANCE->GetVoxelNum() - 1)
    {
        this->SetActorTickEnabled(false);
        UIL_LOG(Warning, TEXT("AGenPrevAssets::Tick: Finished generating preview textures. Other processes may now continue."))
        return;
    }

    ACuboid* Cuboid = this->GetWorld()->SpawnActor<ACuboid>(ACuboid::StaticClass(), this->PrevTransform, FActorSpawnParameters());
    Cuboid->GenerateMesh(this->PrevIndex);

    this->CaptureComponent->TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor::Black, false);
    this->CaptureComponent->TextureTarget->TargetGamma = 1.8f;
    this->CaptureComponent->CaptureScene();

    FImage Image;
    FImageUtils::GetRenderTargetImage(this->CaptureComponent->TextureTarget, Image);

    UTexture2D*         Tex             = FImageUtils::CreateTexture2DFromImage(Image);
    FImage              OutImage        = FImage();
    OutImage.SizeX                      = Image.SizeX;
    OutImage.SizeY                      = Image.SizeY;
    OutImage.NumSlices                  = Image.NumSlices;
    OutImage.Format                     = Image.Format; /* Currently only ERawImageFormat::Type::BGRA8 supported. */
    OutImage.GammaSpace                 = Image.GammaSpace;
    check( OutImage.IsImageInfoValid() )
    const int64         NumPixels       = (int64) (OutImage.SizeX * OutImage.SizeY * OutImage.NumSlices);
    const int64         BytesPerPixel   = AGenPrevAssets::GetBytesPerPixel(OutImage.Format); /* Original: ERawImageFormat::GetBytesPerPixel(OutImage.Format); */
    const int64         NumBytes        = NumPixels * BytesPerPixel;
    OutImage.RawData.Empty(NumBytes);
    OutImage.RawData.AddUninitialized(NumBytes);
    Tex->MipGenSettings                 = TMGS_NoMipmaps;
    FTexture2DMipMap*   Mip             = &Tex->GetPlatformData()->Mips[0];
    const uint32        Width           = Mip->SizeX;
    const uint32        Height          = Mip->SizeY;
    FByteBulkData*      Bulk            = &Mip->BulkData;
    const FColor*       Data            = static_cast<FColor*>(Bulk->Lock(EBulkDataLockFlags::LOCK_READ_ONLY));

    UIL_LOG
    (
        Log,
        TEXT("AGenPrevAssets::Tick: Generating preview texture for %s [%d/%d] (x:%d-y:%d) with raw data size: %d of format %d."),
        *GAME_INSTANCE->GetVoxelName(this->PrevIndex), this->PrevIndex, GAME_INSTANCE->GetVoxelNum(), OutImage.SizeX, OutImage.SizeY, OutImage.RawData.Num(), OutImage.Format
    )

#if WITH_EDITOR
    if (Image.Format != ERawImageFormat::BGRA8)
    {
        UIL_LOG(Fatal, TEXT("AGenPrevAssets::Tick: Unsupported raw image format %d."), Image.Format)
        return;
    }
#endif /* WITH_EDITOR */

    int RawDataCursor = 0;
    for (uint32 h = 0; h < Height; ++h) { for (uint32 w = 0; w < Width; ++w)
    {
        FColor Pixel = Data[w + h * Width];

        if (Pixel == FColor::Black)
        {
            Pixel = FColor::Transparent;
        }

        /* Why does the normal check causes a signed/unsigned mismatch? */
        checkSlow( w < OutImage.SizeX && h < OutImage.SizeY );

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

        OutImage.RawData[RawDataCursor++] = Pixel.B; /* Blue Channel */
        OutImage.RawData[RawDataCursor++] = Pixel.G; /* Green Channel */
        OutImage.RawData[RawDataCursor++] = Pixel.R; /* Red Channel */
        OutImage.RawData[RawDataCursor++] = Pixel.A; /* Alpha Channel */

        continue;	
    }}

    Bulk->Unlock();
    Tex->UpdateResource();

    const FString	FileName	= FString::Printf(TEXT("%s.png"), *GAME_INSTANCE->GetVoxelName(this->PrevIndex));
    const FString	OutPath		= FGeneral::GeneratedAssetsDirectoryRelative / FileName;
    FImageUtils::SaveImageByExtension(*OutPath, OutImage);

    Cuboid->Destroy();

    this->PrevIndex++;

    return;
}

int64 AGenPrevAssets::GetBytesPerPixel(const ERawImageFormat::Type Format)
{
    /*
     * Shamelessly copied from IMAGECORE_API ERawImageFormat::GetBytesPerPixel(ERawImageFormat::Type).
     *
     * This method is rewritten because of a linker error. If this can be resolved, the original method should be used.
     *
     * error LNK2019: unresolved external symbol
     *      "__declspec(dllimport) public: void __cdecl FImage::Init(int,int,enum ERawImageFormat::Type,enum EGammaSpace)"
     *      (__imp_?Init@FImage@@QEAAXHHW4Type@ERawImageFormat@@W4EGammaSpace@@@Z) referenced in function "public: virtual void __cdecl AGenPrevAssets::Tick(float)" (?Tick@AGenPrevAssets@@UEAAXM@Z)
     *      [...]\JAFGv3\unreal\Binaries\Win64\UnrealEditor-JAFG.patch_YXZ.exe : fatal error LNK1120: 1 unresolved externals
     *
     */

    int64 OutBytesPerPixel = 0;

    switch (Format)
    {

    case ERawImageFormat::G8:
        OutBytesPerPixel = 1;
        break;

    case ERawImageFormat::G16:
    case ERawImageFormat::R16F:
        OutBytesPerPixel = 2;
        break;

    case ERawImageFormat::R32F:
    case ERawImageFormat::BGRA8:
    case ERawImageFormat::BGRE8:
        OutBytesPerPixel = 4;
        break;

    case ERawImageFormat::RGBA16:
    case ERawImageFormat::RGBA16F:
        OutBytesPerPixel = 8;
        break;

    case ERawImageFormat::RGBA32F:
        OutBytesPerPixel = 16;
        break;

    default:
        check(0);
        break;

    }

    return OutBytesPerPixel;
}

#undef UIL_LOG
#undef GI
