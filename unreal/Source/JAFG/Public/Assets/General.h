// © 2023 mzoesch. All rights reserved.

#pragma once

#include "Lib/FAccumulated.h"

#include "Core/GI_Master.h"

#include "General.generated.h"

USTRUCT()
struct JAFG_API FGeneral
{
    GENERATED_BODY()

public:
    
    static const inline FString DataDirectoryRelative               = FPaths::ProjectConfigDir() + "Data/";
    static const inline FString DataDirectory                       = FPaths::ConvertRelativePathToFull(FGeneral::DataDirectoryRelative);
    static const inline FString VoxelDataDirectoryRelative          = FGeneral::DataDirectoryRelative + "Voxel/";
    static const inline FString VoxelDataDirectory                  = FPaths::ConvertRelativePathToFull(FGeneral::VoxelDataDirectoryRelative);
    static const inline FString GeneratedAssetsDirectoryRelative    = FPaths::ProjectSavedDir() + "gen/";
    static const inline FString GeneratedAssetsDirectory            = FPaths::ConvertRelativePathToFull(FGeneral::GeneratedAssetsDirectoryRelative);
    
private:

    static UTexture2D* LoadTexture2DFromDisk(const FString& Path);
    
public:
    
    static UTexture2D* LoadTexture2D(const FAccumulated Accumulated, const UGI_Master* GI);
};

