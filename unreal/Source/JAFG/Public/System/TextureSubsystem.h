// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "TextureSubsystem.generated.h"

class UVoxelSubsystem;
JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API UTextureSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    UTextureSubsystem();

    // Subsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~Subsystem implementation

    //////////////////////////////////////////////////////////////////////////
    // Common paths.
    //////////////////////////////////////////////////////////////////////////

    FString GeneratedAssetsDirectoryRelative = ""; //FPaths::ProjectSavedDir() / TEXT("Gen/");
    FString GeneratedAssetsDirectoryAbsolute = ""; //FPaths::ConvertRelativePathToFull(UTextureSubsystem::GeneratedAssetsDirectoryRelative);

    FString RootTextureDirectoryRelative     = ""; //FPaths::ProjectContentDir() / "Assets/Textures/";
    FString RootTextureDirectoryAbsolute     = ""; //FPaths::ConvertRelativePathToFull(UTextureSubsystem::RootTextureDirectoryRelative);

    FString VoxelTextureDirectoryRelative    = ""; //UTextureSubsystem::RootTextureDirectoryRelative / "Voxels/";
    FString VoxelTextureDirectoryAbsolute    = ""; //UTextureSubsystem::RootTextureDirectoryAbsolute / "Voxels/";

    //////////////////////////////////////////////////////////////////////////
    // Specific Textures.
    //////////////////////////////////////////////////////////////////////////

    FString TextureFailureTextureCacheKey                = ""; //"TFT";
    FString TextureFailureTextureFileName                = ""; //"TextureFailure.png";
    FString TextureFailureTextureFilePathAbsolute        = ""; //UTextureSubsystem::RootTextureDirectoryAbsolute / UTextureSubsystem::TextureFailureTextureFileName;
    FString TextureFailureHighResTextureCacheKey         = ""; //"TFTHR";
    FString TextureFailureHighResTextureFileName         = ""; //"TextureFailureHighRes.png";
    FString TextureFailureHighResTextureFilePathAbsolute = ""; //UTextureSubsystem::RootTextureDirectoryAbsolute / UTextureSubsystem::TextureFailureHighResTextureFileName;

    //////////////////////////////////////////////////////////////////////////
    // Interface.
    //////////////////////////////////////////////////////////////////////////

    /**
     * @return The texture associated with the given accumulated data. If the texture is not or failed to load,
     *         a placeholder texture failure will be returned. If the platform blocks the loading of the texture,
     *         nullptr will be returned.
     */
    UTexture2D* GetTexture2D(const FAccumulated& Accumulated);

    /**
     * @return The texture associated with the given name space and texture name. If the texture is not or failed to
     *         load, a placeholder texture failure will be returned. If the platform blocks the loading of the texture,
     *         nullptr will be returned.
     */
    UTexture2D* GetWorldTexture2D(const FString& NameSpace, const FString& TextureName);

private:

    /**
     * For faster access. Cannot change during a session as the Voxel Subsystem is a game instance subsystem.
     */
    TObjectPtr<UVoxelSubsystem> VoxelSubsystem = nullptr;

    UPROPERTY()
    TMap<FString, UTexture2D*>  Cached2DTextures;
    inline static const FString WorldTextureCachePrefix = ""; //"WTC";
    FORCEINLINE auto ClearCached2DTextures(void) -> void { this->Cached2DTextures.Empty(); }

    /**
     * Loads any file from disk, tries to parse the file as a texture and returns the texture.
     * No caching or other optimizations are done here.
     */
    static UTexture2D* LoadTexture2DFromDisk(const FString& AbsolutePath);
};
