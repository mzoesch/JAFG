// © 2023 mzoesch. All rights reserved.

#include "Assets/General.h"

#include "ImageUtils.h"
#include "World/WorldVoxel.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

UTexture2D* FGeneral::LoadTexture2DFromDisk(const FString& Path)
{
    if (!FPaths::FileExists(Path))
    {
        UIL_LOG(Error, TEXT("FGeneral::LoadTexture2DFromDisk - File does not exist: %s"), *Path);
        return nullptr;
    }

    if (Path.Len() > 0)
    {
        FString NormalizedPath = Path;

        NormalizedPath.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);
        NormalizedPath.ReplaceInline(TEXT("//"), TEXT("/"), ESearchCase::CaseSensitive);
        NormalizedPath.RemoveFromStart(TEXT("/"));
        NormalizedPath.RemoveFromEnd(TEXT("/"));
        FPlatformMisc::NormalizePath(NormalizedPath);

        return FImageUtils::ImportFileAsTexture2D(NormalizedPath);
    }

    return nullptr;
}

UTexture2D* FGeneral::LoadTexture2D(const FAccumulated Accumulated, const UGI_Master* GI)
{
    if (Accumulated.GetVoxel() != EWorldVoxel::VoxelNull)
    {
        return FGeneral::LoadTexture2DFromDisk(FString::Printf(TEXT("%s%s.png"), *FGeneral::GeneratedAssetsDirectory, *GI->GetVoxelName(Accumulated.GetVoxel())));
    }
    
    return nullptr;
}
