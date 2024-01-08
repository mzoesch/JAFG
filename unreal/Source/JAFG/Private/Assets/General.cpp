// © 2023 mzoesch. All rights reserved.

#include "Assets/General.h"

#include "ImageUtils.h"

#include "World/WorldVoxel.h"
#include "Core/GI_Master.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define VOXEL_TEXTURE_FILE(NameSpace, Name, Normal) FString::Printf(TEXT("%s/%s%s"), *NameSpace, *Name, *FGeneral::GetNormalSuffix(Normal))
#define UVOXEL_TEXTURE_DIRECTORY(NameSpace, Name, Normal) FString::Printf(TEXT("%s%s"), *FGeneral::UnrealVoxelTextureDirectory, *VOXEL_TEXTURE_FILE(NameSpace, Name, Normal))

void FGeneral::Init(const UGI_Master* GIPtr)
{
    FGeneral::GI = GIPtr;
    FGeneral::Cached2DTextures.Empty();
    return;
}

UTexture2D* FGeneral::LoadTexture2DFromDisk(const FString& Path)
{
    UIL_LOG(Warning, TEXT("FGeneral::LoadTexture2DFromDisk - Loading texture from disk: %s"), *Path);
    
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

ENormalLookup FGeneral::GetNormalLookup(const FVector& Normal)
{
    if (Normal == FVector::UpVector)
    {
        return ENormalLookup::NL_Top;
    }

    if (Normal == FVector::DownVector)
    {
        return ENormalLookup::NL_Bottom;
    }

    if (Normal == FVector::ForwardVector || Normal == FVector::BackwardVector)
    {
        return ENormalLookup::NL_Front;
    }

    if (Normal == FVector::RightVector || Normal == FVector::LeftVector)
    {
        return ENormalLookup::NL_Side;
    }

    return NL_Default;
}

FString FGeneral::GetNormalSuffix(const ENormalLookup Normal)
{
    if (Normal == ENormalLookup::NL_Top)
    {
        return FGeneral::SuffixTop;
    }

    return "";
}

UTexture2D* FGeneral::LoadTexture2D(const FAccumulated Accumulated)
{
    if (Accumulated.GetVoxel() != EWorldVoxel::VoxelNull)
    {
        if (FGeneral::Cached2DTextures.Contains(FGeneral::GI->GetVoxelName(Accumulated.GetVoxel())))
        {
            return FGeneral::Cached2DTextures[FGeneral::GI->GetVoxelName(Accumulated.GetVoxel())];
        }

        if (UTexture2D* Tex =  FGeneral::LoadTexture2DFromDisk(FString::Printf(TEXT("%s%s.png"), *FGeneral::GeneratedAssetsDirectory, *GI->GetVoxelName(Accumulated.GetVoxel()))))
        {
            FGeneral::Cached2DTextures.Add(FGeneral::GI->GetVoxelName(Accumulated.GetVoxel()), Tex);
            return Tex;
        }

        UIL_LOG(Error, TEXT("FGeneral::LoadTexture2D - Failed to load texture for voxel: %s"), *GI->GetVoxelName(Accumulated.GetVoxel()));
        
        return nullptr;
    }
    
    return nullptr;
}

bool FGeneral::ExistsAssetTexture2D(const FString& String, const FString& Name, const ENormalLookup Normal)
{
    return FPaths::FileExists(FString::Printf(TEXT("%s%s.uasset"), *FGeneral::VoxelTextureDirectory, *VOXEL_TEXTURE_FILE(String, Name, Normal)));
}

UTexture2D* FGeneral::LoadAssetTexture2D(const FString& NameSpace, const FString& Name, const ENormalLookup Normal)
{
    return CastChecked<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *UVOXEL_TEXTURE_DIRECTORY(NameSpace, Name, Normal)));
}

#undef UIL_LOG
#undef VOXEL_TEXTURE_FILE
#undef UVOXEL_TEXTURE_DIRECTORY
