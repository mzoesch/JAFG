// © 2023 mzoesch. All rights reserved.

#pragma once

#include "Lib/FAccumulated.h"

#include "World/Voxels/NormalLookup.h"

#include "General.generated.h"

class UGI_Master;

USTRUCT()
struct JAFG_API FGeneral
{
    GENERATED_BODY()

public:

    static const inline FString SuffixTop                           = "Top";
    
public:
    
    static const inline FString GeneratedAssetsDirectoryRelative    = FPaths::ProjectSavedDir() + "gen/";
    static const inline FString GeneratedAssetsDirectory            = FPaths::ConvertRelativePathToFull(FGeneral::GeneratedAssetsDirectoryRelative);

    static const inline FString VoxelTextureDirectory               = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + "Assets/Textures/Voxels/");
    static const inline FString UnrealVoxelTextureDirectory         = "/Game/Assets/Textures/Voxels/";
    
private:

    static inline const UGI_Master*             GI;
    static inline TMap<FString, UTexture2D*>    Cached2DTextures;

public:
    
    static void Init(const UGI_Master* GIPtr);

private:
    
    static UTexture2D*      LoadTexture2DFromDisk(const FString& Path);
    
public:
    
    static ENormalLookup    GetNormalLookup(const FVector& Normal);
    static FString          GetNormalSuffix(const ENormalLookup Normal);

    static UTexture2D*      LoadTexture2D(const FAccumulated Accumulated);
    static bool             ExistsAssetTexture2D(const FString& String, const FString& Name, const ENormalLookup Normal);
    static UTexture2D*      LoadAssetTexture2D(const FString& NameSpace, const FString& Name, const ENormalLookup Normal);
};

