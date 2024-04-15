// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "TextureSubsystem.generated.h"

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

    inline static const FString GeneratedAssetsDirectoryRelative =
        FPaths::ProjectSavedDir() / TEXT("Gen/");
    inline static const FString GeneratedAssetsDirectoryAbsolute =
        FPaths::ConvertRelativePathToFull(UTextureSubsystem::GeneratedAssetsDirectoryRelative);

    inline static const FString VoxelTextureDirectory       =
        FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + "Assets/Textures/Voxels/");
    inline static const FString UnrealVoxelTextureDirectory =
        "/Game/Assets/Textures/Voxels/";

    inline static const FString TestDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() +
            "Assets/Textures/");

    UTexture2D* GetTexture2D(const FAccumulated& Accumulated);

private:

    UPROPERTY()
    TMap<FString, UTexture2D*> Cached2DTextures;
    FORCEINLINE void ClearCached2DTextures(void) { this->Cached2DTextures.Empty(); }

    static UTexture2D* LoadTexture2DFromDisk(const FString& AbsolutePath);
};
