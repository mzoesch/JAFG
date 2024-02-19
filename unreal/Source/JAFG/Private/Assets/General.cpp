// © 2023 mzoesch. All rights reserved.

#include "Assets/General.h"

#include "ImageUtils.h"
#include "Serialization/JsonSerializer.h"

#include "Core/GI_Master.h"

#define UIL_LOG(Verbosity, Format, ...)                     UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define VOXEL_TEXTURE_FILE(NameSpace, Name, Normal)         FString::Printf(TEXT("%s/%s%s"), *NameSpace, *Name, *FGeneral::GetNormalSuffix(Normal))
#define UVOXEL_TEXTURE_DIRECTORY(NameSpace, Name, Normal)   FString::Printf(TEXT("%s%s"), *FGeneral::UnrealVoxelTextureDirectory, *VOXEL_TEXTURE_FILE(NameSpace, Name, Normal))

void FGeneral::Init(const UGI_Master* GIPtr)
{
    FGeneral::GameInstancePtr = GIPtr;
    FGeneral::ClearCached2DTextures();
    return;
}

void FGeneral::NormalizePath(FString& Path)
{
    if (Path.Len() < 1)
    {
        return;
    }

    Path.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);
    Path.ReplaceInline(TEXT("//"), TEXT("/"), ESearchCase::CaseSensitive);
    Path.RemoveFromStart(TEXT("/"));
    Path.RemoveFromEnd(TEXT("/"));

    FPlatformMisc::NormalizePath(Path);

    return;
}

FString FGeneral::LoadStringFromDisk(const FString& Path)
{
    FString NormalizedPath = Path;
    FGeneral::NormalizePath(NormalizedPath);
    
    UIL_LOG(Log, TEXT("FGeneral::LoadStringFromDisk - Loading string from disk: %s."), *NormalizedPath);

    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*NormalizedPath))
    {
        UIL_LOG(Error, TEXT("FGeneral::LoadStringFromDisk - File does not exist: %s."), *NormalizedPath);
        return FString();
    }

    FString Content;

    if (!FFileHelper::LoadFileToString(Content, *NormalizedPath))
    {
        UIL_LOG(Error, TEXT("FGeneral::LoadStringFromDisk - Unable to load and read file: %s."), *NormalizedPath);
        return FString();
    }

    return Content;
}

#pragma region Texture Assets

UTexture2D* FGeneral::LoadTexture2DFromDisk(const FString& Path)
{
    UIL_LOG(Warning, TEXT("FGeneral::LoadTexture2DFromDisk - Loading texture from disk: %s."), *Path);
    
    if (!FPaths::FileExists(Path))
    {
        UIL_LOG(Error, TEXT("FGeneral::LoadTexture2DFromDisk - File does not exist: %s."), *Path);
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

        UTexture2D* Tex = FImageUtils::ImportFileAsTexture2D(NormalizedPath);
        Tex->MipGenSettings = TMGS_NoMipmaps;
        Tex->CompressionSettings = TC_VectorDisplacementmap;
        Tex->SRGB = false;
        Tex->Filter = TextureFilter::TF_Nearest;
        Tex->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
        return Tex;
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

void FGeneral::ClearCached2DTextures()
{
    FGeneral::Cached2DTextures.Empty();
    return;
}

bool FGeneral::AddTexture2DToCache(const FAccumulated Key, UTexture2D* Value)
{
    if (FGeneral::Cached2DTextures.Contains(FGeneral::GameInstancePtr->GetVoxelName(Key.Accumulated)))
    {
        UIL_LOG(Error, TEXT("FGeneral::AddTexture2DToCache - Texture already exists in cache: %s."), *FGeneral::GameInstancePtr->GetVoxelName(Key.Accumulated));
        return false;
    }
    
    FGeneral::Cached2DTextures.Add(FGeneral::GameInstancePtr->GetVoxelName(Key.Accumulated), Value);
    return true;
}

UTexture2D* FGeneral::LoadTexture2D(const FAccumulated Accumulated)
{
    if (Accumulated.Accumulated != FAccumulated::NullAccumulated.Accumulated)
    {
        if (FGeneral::Cached2DTextures.Contains(FGeneral::GameInstancePtr->GetVoxelName(Accumulated.Accumulated)))
        {
            return FGeneral::Cached2DTextures[FGeneral::GameInstancePtr->GetVoxelName(Accumulated.Accumulated)];
        }

        if (Accumulated.IsVoxel())
        {
            if (UTexture2D* Tex = FGeneral::LoadTexture2DFromDisk(FString::Printf(TEXT("%s%s.png"), *FGeneral::GeneratedAssetsDirectory, *GameInstancePtr->GetVoxelName(Accumulated.Accumulated))))
            {
                FGeneral::Cached2DTextures.Add(FGeneral::GameInstancePtr->GetVoxelName(Accumulated.Accumulated), Tex);
                return Tex;
            }
        }
        else
        {
            if (UTexture2D* Tex = FGeneral::LoadTexture2DFromDisk(FString::Printf(TEXT("%s%s.png"), *FGeneral::ItemTextureDirectory, *GameInstancePtr->GetVoxelName(Accumulated.Accumulated))))
            {
                FGeneral::Cached2DTextures.Add(FGeneral::GameInstancePtr->GetVoxelName(Accumulated.Accumulated), Tex);
                return Tex;
            }
        }
        
        UIL_LOG(Error, TEXT("FGeneral::LoadTexture2D - Failed to load texture for voxel: %s"), *GameInstancePtr->GetVoxelName(Accumulated.Accumulated));
        
        return nullptr;
    }
    
    return nullptr;
}

UTexture2D* FGeneral::LoadTexture2D(const FString& NameSpace, const FString& Name)
{
    return FGeneral::LoadTexture2DFromDisk(FString::Printf(TEXT("%s%s/%s.png"), *FGeneral::VoxelTextureDirectory, *NameSpace, *Name));
}

TArray<FString> FGeneral::GetAllVoxelTextureNames(const FString& NameSpace)
{
    TArray<FString> TextureNames;
    IFileManager::Get().FindFiles(TextureNames, *FString::Printf(TEXT("%s%s"), *FGeneral::VoxelTextureDirectory, *NameSpace), TEXT("png"));

    for (FString& TextureName : TextureNames)
    {
        TextureName.RemoveFromEnd(TEXT(".png"));
        continue;
    }

    return TextureNames;
}

bool FGeneral::ExistsAssetTexture2D(const FString& String, const FString& Name, const ENormalLookup Normal)
{
    return FPaths::FileExists(FString::Printf(TEXT("%s%s.uasset"), *FGeneral::VoxelTextureDirectory, *VOXEL_TEXTURE_FILE(String, Name, Normal)));
}

UTexture2D* FGeneral::LoadAssetTexture2D(const FString& NameSpace, const FString& Name, const ENormalLookup Normal)
{
    return CastChecked<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *UVOXEL_TEXTURE_DIRECTORY(NameSpace, Name, Normal)));
}

#pragma endregion Texture Assets

#pragma region Prescriptions

TSharedPtr<FJsonObject> FGeneral::LoadPrescriptionFromDisk(const FString& Path)
{
    UIL_LOG(Log, TEXT("FGeneral::LoadPrescriptionFromDisk - Loading prescription from disk: %s."), *Path);

    const FString PrescriptionContent = FGeneral::LoadStringFromDisk(Path);

    if (PrescriptionContent.Len() < 1)
    {
        UIL_LOG(Fatal, TEXT("FGeneral::LoadPrescriptionFromDisk - Unable to load prescription from disk: %s."), *Path);
        return nullptr;
    }

    TSharedPtr<FJsonObject> RetrievedJSONObject;

    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(PrescriptionContent), RetrievedJSONObject) || !RetrievedJSONObject.IsValid())
    {
        UIL_LOG(Fatal, TEXT("FGeneral::LoadPrescriptionFromDisk - Unable to deserialize prescription from disk: %s. The found content was %s."), *Path, *PrescriptionContent);
        return nullptr;
    }

    return RetrievedJSONObject;
}

TSharedPtr<FJsonObject> FGeneral::LoadPrescription(const FString& PrescriptionName)
{
    return FGeneral::LoadPrescriptionFromDisk(FString::Printf(TEXT("%s%s.json"), *FGeneral::PrescriptionDirectory, *PrescriptionName));
}

TArray<FString> FGeneral::LoadPrescriptionNames()
{
    UIL_LOG(Log, TEXT("FGeneral::LoadPrescriptionNames - Searching for all available prescriptions in: %s."), *FGeneral::PrescriptionDirectory);
    
    TArray<FString> PrescriptionNames;
    
    IFileManager::Get().FindFiles(PrescriptionNames, *FGeneral::PrescriptionDirectory, TEXT("json"));

    for (FString& PrescriptionName : PrescriptionNames)
    {
        PrescriptionName.RemoveFromEnd(TEXT(".json"));
        continue;
    }
    
    UIL_LOG(Warning, TEXT("FGeneral::LoadPrescriptionNames - Found %d prescriptions."), PrescriptionNames.Num())
    
    return PrescriptionNames;
}

#pragma endregion Prescriptions

#undef UIL_LOG
#undef VOXEL_TEXTURE_FILE
#undef UVOXEL_TEXTURE_DIRECTORY
