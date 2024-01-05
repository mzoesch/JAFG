// © 2023 mzoesch. All rights reserved.

#pragma once

#include "Lib/FAccumulated.h"

#include "General.generated.h"

class UGI_Master;

USTRUCT()
struct JAFG_API FGeneral
{
    GENERATED_BODY()

public:
    
    static const inline FString DataDirectoryRelative               = FPaths::ProjectContentDir() + "Data/";
    static const inline FString DataDirectory                       = FPaths::ConvertRelativePathToFull(FGeneral::DataDirectoryRelative);

    static const inline FString VoxelDataDirectoryRelative          = FGeneral::DataDirectoryRelative + "Voxel/";
    static const inline FString VoxelDataDirectory                  = FPaths::ConvertRelativePathToFull(FGeneral::VoxelDataDirectoryRelative);

    static const inline FString GeneratedAssetsDirectoryRelative    = FPaths::ProjectSavedDir() + "gen/";
    static const inline FString GeneratedAssetsDirectory            = FPaths::ConvertRelativePathToFull(FGeneral::GeneratedAssetsDirectoryRelative);

    static const inline FString TextureDirectoryRelative            = FPaths::ProjectContentDir() + "Texture/";
    static const inline FString TextureDirectory                    = FPaths::ConvertRelativePathToFull(FGeneral::TextureDirectoryRelative);
    
private:

    static inline const UGI_Master*             GI;
    static inline TMap<FString, UTexture2D*>    Cached2DTextures;

public:
    
    static void Init(const UGI_Master* GIPtr);

private:
    
    static UTexture2D* LoadTexture2DFromDisk(const FString& Path);
    
public:

    static UTexture2D* LoadTexture2D(const FAccumulated Accumulated);
};

