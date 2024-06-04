// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExternalSubsystem.h"
#include "JAFGLogDefs.h"
#include "JAFGTypeDefs.h"

#include "TextureSubsystem.generated.h"

class UVoxelSubsystem;
struct FAccumulated;

namespace ESubNameSpacePaths
{

enum Type : uint8
{
    Invalid     = 0,
    Voxels      = 1,
    Blends      = 2,
    Destruction = 3,
    GUI         = 4,

    Generated   = 0xFF,
};

}

FORCEINLINE FString LexToString(const ESubNameSpacePaths::Type InType)
{
    switch (InType)
    {
    case ESubNameSpacePaths::Voxels:
    {
        return TEXT("Voxels");
    }
    case ESubNameSpacePaths::Blends:
    {
        return TEXT("Blends");
    }
    case ESubNameSpacePaths::Destruction:
    {
        return TEXT("Destruction");
    }
    case ESubNameSpacePaths::GUI:
    {
        return TEXT("GUI");
    }
    default:
    {
        LOG_FATAL(LogTextureSubsystem, "Invalid type was provided: %d.", static_cast<int32>(InType))
        return TEXT("Invalid");
    }
    }
}

#define DECLARE_NAMED_TEXTURE(Name) \
    static const FString Name = TEXT(#Name);

namespace NamedTextures
{

DECLARE_NAMED_TEXTURE(GeneralContainerSlot)

}

UCLASS(NotBlueprintable)
class JAFGEXTERNALCORE_API UTextureSubsystem : public UExternalSubsystem
{
    GENERATED_BODY()

public:

    UTextureSubsystem();

    // Subsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~Subsystem implementation

    FString TexSectionDivider     = L"";
    TCHAR   TexSectionDividerChar = '_';

    FString FileExtension         = L"";

    //////////////////////////////////////////////////////////////////////////
    // Common paths.
    //////////////////////////////////////////////////////////////////////////

    FString GeneratedAssetsDirectoryRelative    = L"";
    FString GeneratedAssetsDirectoryAbsolute    = L"";

    FString RootAssetsDirectoryRelative         = L"";
    FString RootAssetsDirectoryAbsolute         = L"";

    FString RootTextureDirectoryRelative        = L"";
    FString RootTextureDirectoryAbsolute        = L"";

    FString VoxelTextureDirectoryRelative       = L"";
    FString VoxelTextureDirectoryAbsolute       = L"";

    FString BlendTextureDirectoryRelative       = L"";
    FString BlendTextureDirectoryAbsolute       = L"";

    auto CreatePath(const FString& InNameSpace, const ESubNameSpacePaths::Type InType) const -> FString;
    auto CreatePathFile(const FString& InNameSpace, const ESubNameSpacePaths::Type InType, const FString& InName) const -> FString;

    //////////////////////////////////////////////////////////////////////////
    // Specific Textures.
    //////////////////////////////////////////////////////////////////////////

    FString TextureFailureTextureFileName                = "";
    FString TextureFailureTextureFilePathAbsolute        = "";
    FString TextureFailureHighResTextureFileName         = "";
    FString TextureFailureHighResTextureFilePathAbsolute = "";

    //////////////////////////////////////////////////////////////////////////
    // Interface.
    //////////////////////////////////////////////////////////////////////////

    auto GetAndCacheTexture2D(const FString& CacheKey, const FString& FallbackAbsoluteFilePath) -> UTexture2D*;

    auto GetGUITexture2D(const FString& TextureName) -> UTexture2D*;
    auto GetSafeGUITexture2D(const FString& TextureName) -> UTexture2D*;

    UTexture2D* GetPreviewTexture2D(const voxel_t AccumulatedIndex);

    auto GetTexture2D(const voxel_t AccumulatedIndex) -> UTexture2D*;
    auto GetSafeTexture2D(const voxel_t AccumulatedIndex) -> UTexture2D*;

    /**
     * Checks all files in the blend texture directory and returns the names of all textures found.
     * Will not load the actual textures or cache them.
     */
    auto LoadAllBlendTextureNames(void) const -> TArray<FString>;
    auto LoadAllDestructionTextureNames(const FString& NameSpace) const -> TArray<FString>;

    /**
     * @return The texture associated with the blend name. If the texture is not found or failed to load, a placeholder
     *         texture failure will be returned. If the platform blocks the loading of the texture, nullptr will be
     *         returned.
     */
    auto GetBlendTexture2D(const FString& BlendName) -> UTexture2D*;
    auto GetSafeBlendTexture2D(const FString& BlendName) -> UTexture2D*;

    /** Will load all texture names for the given name space into memory. */
    auto LoadTextureNamesForNamespace(const FString& NameSpace) -> void;
    /** @return The number of textures found for the given name space. */
    auto GetWorldTexture2DCount(const FString& NameSpace) -> int32;
    /** @return The texture name at the given index for the given name space. Sorted in alphabetical order. */
    auto GetWorldTexture2DNameByIndex(const FString& NameSpace, const int32 Index) -> const FString&;
    /** @return The texture names for the given name space. */
    auto GetWorldTextureNamesForNamespace(const FString& InNameSpace) -> const TArray<FString>&;

    /**
     * @return The texture associated with the given name space and texture name. If the texture is not or failed to
     *         load, a placeholder texture failure will be returned. If the platform blocks the loading of the texture,
     *         nullptr will be returned.
     */
    UTexture2D* GetWorldTexture2D(const FString& NameSpace, const FString& TextureName);

    UTexture2D* GetWorldDestructionTexture2D(const FString& NameSpace, const FString& TextureName);
    UTexture2D* GetSafeWorldDestructionTexture2D(const FString& NameSpace, const FString& TextureName);

    //////////////////////////////////////////////////////////////////////////
    // MISC.
    //////////////////////////////////////////////////////////////////////////

    static auto GetBytesPerPixel(const ERawImageFormat::Type Format) -> int64;

    /**
     * Given any valid texture name, it will split the name up into their respective parts.
     * UTextureSubsystem#TexSectionDivider is used to split the name.
     * See MaterialSubsystem.h for more information about how to properly name textures.
     */
    auto SplitTextureName(const FString& TextureName) const -> TArray<FString>;

private:

    /** For faster access. Cannot change during a session as the Voxel Subsystem is a game instance subsystem. */
    UPROPERTY()
    TObjectPtr<UVoxelSubsystem> VoxelSubsystem = nullptr;

    /** Maps a name space to all the texture names found in the appropriate directory for that name space. */
    struct FPrivateTexNames
    {
        FString         NameSpace;
        TArray<FString> TextureNames;
    };
    /** The world texture names that where found when first interacted with the given name space. */
    TArray<FPrivateTexNames> WorldTextureNames;
    /** Kinda sketchy solution. But works. See the implementation of UTextureSubsystem#GetWorldTextureNamesForNamespace. */
    const TArray<FString>    EmptyStringArray;
    /** @return True if the given name space's texture names have already been loaded into memory. */
    FORCEINLINE bool HasLoadedTextureNamesForNameSpace(const FString& InNameSpace) const
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
    TMap<FString, UTexture2D*> Cached2DTextures;
    FString TextureFailureTextureCacheKey        = "";
    FString TextureFailureHighResTextureCacheKey = "";
    FString WorldTextureCachePrefix              = "";
    FString BlendTextureCachePrefix              = "";
    FORCEINLINE auto ClearCached2DTextures(void) -> void { this->Cached2DTextures.Empty(); }

    /**
     * Loads any file from disk, tries to parse the file as a texture and returns the texture.
     * No caching or other optimizations are done here.
     */
    static UTexture2D* LoadTexture2DFromDisk(const FString& AbsolutePath);
};
