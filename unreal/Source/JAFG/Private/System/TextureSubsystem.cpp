// Copyright 2024 mzoesch. All rights reserved.

#include "System/TextureSubsystem.h"

#include "ImageUtils.h"
#include "Jar/Accumulated.h"
#include "World/Voxel/VoxelSubsystem.h"

UTextureSubsystem::UTextureSubsystem(const FObjectInitializer& ObjectInitializer) /* : Super(ObjectInitializer) */
{
    /* The Object Initializer is not necessary for this class as it is not exposed to Kismet. */

    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ClearCached2DTextures();

    return;
}

void UTextureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    /* This subsystem is dependent. */
    Collection.InitializeDependency<UVoxelSubsystem>();

    Super::Initialize(Collection);

    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ClearCached2DTextures();

    check( this->GetGameInstance() )
    this->VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    check( this->VoxelSubsystem )

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
    check( this->VoxelSubsystem )

#if WITH_EDITOR
    if (Accumulated.AccumulatedIndex < this->VoxelSubsystem->GetCommonVoxelNum())
    {
        LOG_FATAL(LogTemp, "Texture requested for a common voxel. This is disallowed. Requested Accumulated: %s.", *Accumulated.ToString())
        return nullptr;
    }
#endif /* WITH_EDITOR */

    if (this->Cached2DTextures.Contains(this->VoxelSubsystem->GetVoxelName(Accumulated.AccumulatedIndex)))
    {
        return this->Cached2DTextures[this->VoxelSubsystem->GetVoxelName(Accumulated.AccumulatedIndex)];
    }

    if (UTexture2D* Tex =
        UTextureSubsystem::LoadTexture2DFromDisk(
            FString::Printf(
                TEXT("%s.png"),
                * ( UTextureSubsystem::GeneratedAssetsDirectoryAbsolute / this->VoxelSubsystem->GetVoxelName(Accumulated.AccumulatedIndex) )
            )
        )
    )
    {
        this->Cached2DTextures.Add(this->VoxelSubsystem->GetVoxelName(Accumulated.AccumulatedIndex), Tex);
        /* Safety net. If everything worked accordingly. */
        return this->GetTexture2D(Accumulated);
    }

    /* Failed to find texture. Returning a placeholder to not immediately crash the game. */

    if (this->Cached2DTextures.Contains(UTextureSubsystem::TextureFailureTextureCacheKey))
    {
        return this->Cached2DTextures[UTextureSubsystem::TextureFailureTextureCacheKey];
    }

    if (UTexture2D* Tex = UTextureSubsystem::LoadTexture2DFromDisk(UTextureSubsystem::TextureFailureTextureFilePathAbsolute))
    {
        this->Cached2DTextures.Add(UTextureSubsystem::TextureFailureTextureCacheKey, Tex);
        /* Safety net. If everything worked accordingly. */
        return this->GetTexture2D(Accumulated);
    }

    LOG_ERROR(LogTemp, "Failed to load texture for accumulated %s and failed to load the placeholder texture.", *Accumulated.ToString())

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
