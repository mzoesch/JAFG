// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExternalGameInstanceSubsystem.h"
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

    Destruction,
    GUI,
    Voxels,
    Items,
    Blends,

    Generated   = 0xFF,
};

}

FORCEINLINE FString LexToString(const ESubNameSpacePaths::Type InType)
{
    switch (InType)
    {
    case ESubNameSpacePaths::Destruction:
    {
        return TEXT("Destruction");
    }
    case ESubNameSpacePaths::GUI:
    {
        return TEXT("GUI");
    }
    case ESubNameSpacePaths::Voxels:
    {
        return TEXT("Voxels");
    }
    case ESubNameSpacePaths::Items:
    {
        return TEXT("Items");
    }
    case ESubNameSpacePaths::Blends:
    {
        return TEXT("VoxelsAlpha");
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
DECLARE_NAMED_TEXTURE(HotbarSelector)

}

UCLASS(NotBlueprintable)
class JAFGEXTERNALCORE_API UTextureSubsystem : public UExternalGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    UTextureSubsystem();

    // Subsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~Subsystem implementation

    TCHAR   TexSectionDividerChar = '_';
    FString TexSectionDivider     = TCHAR_TO_UTF8(&TexSectionDividerChar);

    FString FileExtension         = TEXT("png");

    //////////////////////////////////////////////////////////////////////////
    // Common paths.
    //////////////////////////////////////////////////////////////////////////

    FString GeneratedAssetsDirectoryRelative    = TEXT("");
    FString GeneratedAssetsDirectoryAbsolute    = TEXT("");

    FString RootTextureDirectoryRelative        = TEXT("");
    FString RootTextureDirectoryAbsolute        = TEXT("");

    //////////////////////////////////////////////////////////////////////////
    // Specific Textures.
    //////////////////////////////////////////////////////////////////////////

    FString TextureFailureTextureFileName                = TEXT("TextureFailure.png");
    FString TextureFailureTextureFilePathAbsolute        = TEXT("");
    FString TextureFailureHighResTextureFileName         = TEXT("TextureFailureHighRes.png");
    FString TextureFailureHighResTextureFilePathAbsolute = TEXT("");

    //////////////////////////////////////////////////////////////////////////
    // Interface.
    //////////////////////////////////////////////////////////////////////////

    /**
     * Create an absolute path to a specific texture file in a namespace.
     * @return True if the file exists.
     */
    bool CreatePathToFile(const FString& InNamespace, const ESubNameSpacePaths::Type InType, const FString& InFileName, FString& OutAbsolutePath) const;
    /**
     * Create an absolute path to a specific texture based on the user preferences for currently loaded namespaces.
     * @return True, if the file exists and the absolute path is meaningful.
     */
    bool CreatePathToFile(const ESubNameSpacePaths::Type InType, const FString& InFileName, FString& OutAbsolutePath) const;
    FString GetFirstNamespaceForFile(const ESubNameSpacePaths::Type InType, const FString& InFileName) const;
    /** @return True, if the directory exists. */
    bool CreatePathToDirectory(const FString& InNamespace, const ESubNameSpacePaths::Type InType, FString& OutAbsolutePath) const;

    const TArray<FString>& GetUsedTextureNamespaces(void) const { return this->UsedTextureNamespaces; }

    UTexture2D* GetGUITexture2D(const FString& TextureName);
    UTexture2D* GetSafeGUITexture2D(const FString& TextureName);

    const FString PreviewCachePrefix = TEXT("PR_");
    UTexture2D* GetPreviewTexture2D(const voxel_t AccumulatedIndex);
    UTexture2D* GetSafePreviewTexture2D(const voxel_t AccumulatedIndex);

    UTexture2D* GetVoxelTexture2D(const FString& TextureName);
    UTexture2D* GetSafeVoxelTexture2D(const FString& TextureName);

    TArray<FString> LoadAllBlendTextureNames(void) const;
    UTexture2D* GetBlendTexture2D(const FString& BlendName);
    UTexture2D* GetSafeBlendTexture2D(const FString& BlendName);

    TArray<FString> LoadAllDestructionTextureNames(void) const;
    UTexture2D* GetDestructionTexture2D(const FString& TextureName);
    UTexture2D* GetSafeDestructionTexture2D(const FString& TextureName);

    TArray<FString> LoadAllVoxelTextureNamesForNamespace(const FString& Namespace) const;
    TArray<FString> SplitVoxelTextureName(const FString& TextureName) const;

    //////////////////////////////////////////////////////////////////////////
    // MISC.
    //////////////////////////////////////////////////////////////////////////

    static auto GetBytesPerPixel(const ERawImageFormat::Type Format) -> int64;

private:

    /** For faster access. Cannot change during a session as the Voxel Subsystem is a game instance subsystem. */
    UPROPERTY()
    TObjectPtr<UVoxelSubsystem> VoxelSubsystem = nullptr;

    /**
     * Currently used texture namespaces. Defined by the user.
     * In order of priority (Highest to lowest).
     */
    TArray<FString> UsedTextureNamespaces;

    UPROPERTY()
    TMap<FString, UTexture2D*> Cached2DTextures;
    FORCEINLINE auto ClearCached2DTextures(void) -> void { this->Cached2DTextures.Empty(); }

    UTexture2D* GetAndCacheTexture2D(const FString& CacheKey, const FString& FallbackAbsoluteFilePath);
    /**
     * Loads any file from disk, tries to parse the file as a texture and returns the texture.
     * No caching or other optimizations are done here.
     */
    static UTexture2D* LoadTexture2DFromDisk(const FString& AbsolutePath);
};
