// © 2023 mzoesch. All rights reserved.

#include "Assets/General.h"

#include "ImageUtils.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

UTexture2D* FGeneral::LoadTexture2DFromDisk(const FString& Path)
{
    if (!FPaths::FileExists(Path))
    {
        UIL_LOG(Error, TEXT("FGeneral::LoadTexture2DFromDisk - File does not exist: %s"), *Path);
        return nullptr;
    }

    const FString File = FString::Printf(TEXT("gen/Render.png"));
    
    FString SavedPath = FPaths::ProjectSavedDir();
    FString FullSavedPath = FPaths::ConvertRelativePathToFull(SavedPath);
    FString FullPath = FullSavedPath + File;
    UIL_LOG(Error, TEXT("FGeneral::LoadTexture2DFromDisk - FullPath: %s"), *FullPath);
    UTexture2D* Texture = nullptr;
    if (FullPath.Len() > 0) {
        FullPath.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);
        FullPath.ReplaceInline(TEXT("//"), TEXT("/"), ESearchCase::CaseSensitive);
        FullPath.RemoveFromStart(TEXT("/"));
        FullPath.RemoveFromEnd(TEXT("/"));
        FPlatformMisc::NormalizePath(FullPath);
        UIL_LOG(Error, TEXT("FGeneral::LoadTexture2DFromDisk - FullPath norm: %s"), *FullPath);
        Texture = FImageUtils::ImportFileAsTexture2D(FullPath);
    }

    return Texture;
}

UTexture2D* FGeneral::LoadTexture2D(const FAccumulated Accumulated)
{
    if (Accumulated.GetVoxel() != EVoxel::Null)
    {
        const FString Path = FString::Printf(TEXT("%sRender.png"), *FGeneral::GeneratedAssetsDirectory);
        // const FString Path = FString::Printf(TEXT("E:\\dev\\ue\\prj_bi\\JAFGv3\\unreal\\Saved\\gen\\Render.png"));
        return FGeneral::LoadTexture2DFromDisk(Path);
    }
    
    return nullptr;
}
