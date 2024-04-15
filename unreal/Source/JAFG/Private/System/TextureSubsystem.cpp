// Copyright 2024 mzoesch. All rights reserved.

#include "System/TextureSubsystem.h"

#include "ImageUtils.h"

UTextureSubsystem::UTextureSubsystem(const FObjectInitializer& ObjectInitializer) /* : Super(ObjectInitializer) */
{
    /* The Object Initializer is not necessary for this class as it is not exposed to Kismet. */

    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ClearCached2DTextures();

    return;
}

void UTextureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ClearCached2DTextures();

    return;
}

void UTextureSubsystem::Deinitialize(void)
{
    Super::Deinitialize();

    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ClearCached2DTextures();

    return;
}

UTexture2D* UTextureSubsystem::GetTexture2D(const FAccumulated& Accumulated)
{
    if (UTexture2D* Tex = UTextureSubsystem::LoadTexture2DFromDisk(FString::Printf(TEXT("%s.png"),
        *(UTextureSubsystem::TestDir / "TextureFailure"))))
    {
        UE_LOG(LogTemp, Warning, TEXT("UTextureSubsystem::GetTexture2D: Texture loaded successfully."));
    return Tex;
    }

    UE_LOG(LogTemp, Error, TEXT("UTextureSubsystem::GetTexture2D: Failed to load texture."));
    return nullptr;
}

UTexture2D* UTextureSubsystem::LoadTexture2DFromDisk(const FString& AbsolutePath)
{
    UE_LOG(LogTemp, Warning, TEXT("UTextureSubsystem::LoadTexture2DFromDisk: Loading texture from disk: %s."), *AbsolutePath);

    if (!FPaths::FileExists(AbsolutePath))
    {
        UE_LOG(LogTemp, Error, TEXT("UTextureSubsystem::LoadTexture2DFromDisk: File does not exist: %s."), *AbsolutePath);
        return nullptr;
    }

    if (AbsolutePath.Len() <= 0)
    {
        return nullptr;
    }

    FString NormalizedPath = AbsolutePath;

    NormalizedPath.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);
    NormalizedPath.ReplaceInline(TEXT("//"), TEXT("/"), ESearchCase::CaseSensitive);
    NormalizedPath.RemoveFromStart(TEXT("/"));
    NormalizedPath.RemoveFromEnd(TEXT("/"));
    FPlatformMisc::NormalizePath(NormalizedPath);

    UTexture2D* Tex = FImageUtils::ImportFileAsTexture2D(NormalizedPath);
    Tex->MipGenSettings = TMGS_NoMipmaps;
    Tex->CompressionSettings = TC_VectorDisplacementmap;
    Tex->SRGB = false;
    Tex->Filter = TextureFilter::TF_Nearest;
    Tex->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;

    return Tex;
}
