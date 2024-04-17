// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "TextureSubsystem.generated.h"

class UVoxelSubsystem;
struct FAccumulated;
JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API UTextureSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    explicit UTextureSubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // Subsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~Subsystem implementation

public:

    //////////////////////////////////////////////////////////////////////////
    // Common paths.
    //////////////////////////////////////////////////////////////////////////

    inline static const FString GeneratedAssetsDirectoryRelative =
        FPaths::ProjectSavedDir() / TEXT("Gen/");
    inline static const FString GeneratedAssetsDirectoryAbsolute =
        FPaths::ConvertRelativePathToFull(UTextureSubsystem::GeneratedAssetsDirectoryRelative);

    inline static const FString RootTextureDirectoryRelative =
        FPaths::ProjectContentDir() + "Assets/Textures/";
    inline static const FString RootTextureDirectoryAbsolute =
        FPaths::ConvertRelativePathToFull(UTextureSubsystem::RootTextureDirectoryRelative);

    //////////////////////////////////////////////////////////////////////////
    // Specific Textures.
    //////////////////////////////////////////////////////////////////////////

    inline static const FString TextureFailureTextureCacheKey =
        "TFT";
    inline static const FString TextureFailureTextureFileName =
        "TextureFailure.png";
    inline static const FString TextureFailureTextureFilePathAbsolute =
        UTextureSubsystem::RootTextureDirectoryAbsolute / UTextureSubsystem::TextureFailureTextureFileName;

    inline static const FString VoxelTextureDirectory       =
        FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + "Assets/Textures/Voxels/");
    inline static const FString UnrealVoxelTextureDirectory =
        "/Game/Assets/Textures/Voxels/";

    //////////////////////////////////////////////////////////////////////////
    // Interface.
    //////////////////////////////////////////////////////////////////////////

    /**
     * @return The texture associated with the given accumulated data. If the texture is not or failed to load,
     *         a placeholder texture failure will be returned. If the OS blocks the loading of the texture,
     *         nullptr will be returned.
     */
    UTexture2D* GetTexture2D(const FAccumulated& Accumulated);

private:

    TObjectPtr<UVoxelSubsystem> VoxelSubsystem = nullptr;

    UPROPERTY()
    TMap<FString, UTexture2D*> Cached2DTextures;
    FORCEINLINE void ClearCached2DTextures(void) { this->Cached2DTextures.Empty(); }

    static UTexture2D* LoadTexture2DFromDisk(const FString& AbsolutePath);
};
