// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "TextureSubsystem.generated.h"

JAFG_VOID

class UVoxelSubsystem;

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

    FString TexSectionDivider = "";
    TCHAR   TexSectionDividerChar = '_';

    //////////////////////////////////////////////////////////////////////////
    // Common paths.
    //////////////////////////////////////////////////////////////////////////

    FString GeneratedAssetsDirectoryRelative = "";
    FString GeneratedAssetsDirectoryAbsolute = "";

    FString RootTextureDirectoryRelative     = "";
    FString RootTextureDirectoryAbsolute     = "";

    FString VoxelTextureDirectoryRelative    = "";
    FString VoxelTextureDirectoryAbsolute    = "";

    FString BlendTextureDirectoryRelative    = "";
    FString BlendTextureDirectoryAbsolute    = "";

    //////////////////////////////////////////////////////////////////////////
    // Specific Textures.
    //////////////////////////////////////////////////////////////////////////

    FString TextureFailureTextureCacheKey                = "";
    FString TextureFailureTextureFileName                = "";
    FString TextureFailureTextureFilePathAbsolute        = "";
    FString TextureFailureHighResTextureCacheKey         = "";
    FString TextureFailureHighResTextureFileName         = "";
    FString TextureFailureHighResTextureFilePathAbsolute = "";

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
     * Checks all files in the blend texture directory and returns the names of all textures found.
     * Will not load the actual textures or cache them.
     */
    TArray<FString> LoadAllBlendTextureNames(void) const;

    /**
     * @return The texture associated with the given name space and texture name. If the texture is not or failed to
     *         load, a placeholder texture failure will be returned. If the platform blocks the loading of the texture,
     *         nullptr will be returned.
     */
    UTexture2D* GetBlendTexture2D(const FString& BlendName);

    auto LoadTextureNamesForNamespace(const FString& NameSpace) -> void;
    auto GetWorldTexture2DCount(const FString& NameSpace) -> int32;
    auto GetWorldTexture2DNameByIndex(const FString& NameSpace, const int32 Index) const -> const FString&;
    auto GetWorldTextureNamesForNamespace(const FString& InNameSpace) const -> const TArray<FString>&;

    /**
     * @return The texture associated with the given name space and texture name. If the texture is not or failed to
     *         load, a placeholder texture failure will be returned. If the platform blocks the loading of the texture,
     *         nullptr will be returned.
     */
    UTexture2D* GetWorldTexture2D(const FString& NameSpace, const FString& TextureName);

    //////////////////////////////////////////////////////////////////////////
    // MISC.
    //////////////////////////////////////////////////////////////////////////

    static int64 GetBytesPerPixel(const ERawImageFormat::Type Format);

private:

    /**
     * For faster access. Cannot change during a session as the Voxel Subsystem is a game instance subsystem.
     */
    UPROPERTY()
    TObjectPtr<UVoxelSubsystem> VoxelSubsystem = nullptr;

    struct FPrivateTexNames
    {
        FString         NameSpace;
        TArray<FString> TextureNames;
    };

    /**
     * The world texture names that where found when first interacted with the given name space.
     */
    TArray<FPrivateTexNames> WorldTextureNames;
    /** Kinda sketchy solution. But works. See the implementation of UTextureSubsystem#GetWorldTextureNamesForNamespace. */
    const TArray<FString>    EmptyStringArray;
    FORCEINLINE bool HasWorldTextureNames(const FString& InNameSpace) const
    {
        for (const auto& [NameSpace, TextureNames] : this->WorldTextureNames)
        {
            if (NameSpace == InNameSpace)
            {
                return true;
            }
        }

        return false;
    }

    UPROPERTY()
    TMap<FString, UTexture2D*>  Cached2DTextures;
    FString WorldTextureCachePrefix = "";
    FString BlendTextureCachePrefix = "";
    FORCEINLINE auto ClearCached2DTextures(void) -> void { this->Cached2DTextures.Empty(); }

    /**
     * Loads any file from disk, tries to parse the file as a texture and returns the texture.
     * No caching or other optimizations are done here.
     */
    static UTexture2D* LoadTexture2DFromDisk(const FString& AbsolutePath);
};
