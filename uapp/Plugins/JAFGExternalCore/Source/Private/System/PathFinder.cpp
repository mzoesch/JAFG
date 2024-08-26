// Copyright 2024 mzoesch. All rights reserved.

#include "System/PathFinder.h"
#include "JAFGMacros.h"
#include "Interfaces/IPluginManager.h"
#include "Dom/JsonObject.h"

FString LexToString(const EPathType::Type InType)
{
    switch (InType)
    {
    case EPathType::Destruction:
    {
        return TEXT("Assets/Textures/Destruction");
    }
    case EPathType::GUI:
    {
        return TEXT("Assets/Textures/GUI");
    }
    case EPathType::Items:
    {
        return TEXT("Assets/Textures/Items");
    }
    case EPathType::Voxels:
    {
        return TEXT("Assets/Textures/Voxels");
    }
    case EPathType::VoxelsAlpha:
    {
        return TEXT("Assets/Textures/VoxelsAlpha");
    }
    case EPathType::Recipes:
    {
        return TEXT("Assets/Recipes");
    }
    case EPathType::Fonts:
    {
        return TEXT("Assets/Fonts");
    }
    default:
    {
        jcheckNoEntry()
        return TEXT("");
    }
    }
}

void PathFinder::RelativeToAbsolutePath(FString& Path)
{
    Path = FPaths::ConvertRelativePathToFull(Path);
}

void PathFinder::RelativeToAbsolutePath(const IPlugin* InPlugin, const FString& InRelativePath, FString& OutAbsolutePath)
{
    PathFinder::CreatePathToFile(InPlugin, InRelativePath, OutAbsolutePath);
    PathFinder::RelativeToAbsolutePath(OutAbsolutePath);
}

void PathFinder::RelativeToAbsolutePath(const IPlugin* InPlugin, const EPathType::Type InPathType, const FString& InFileName, FString& OutAbsolutePath)
{
    PathFinder::CreatePathToFile(InPlugin, InPathType, InFileName, OutAbsolutePath);
    PathFinder::RelativeToAbsolutePath(OutAbsolutePath);
}

void PathFinder::NormalizePath(FString& Path)
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

bool PathFinder::DoesPathExist(const FString& Path)
{
    if (IFileManager::Get().DirectoryExists(*Path))
    {
        return true;
    }

    return IFileManager::Get().FileExists(*Path);
}

bool PathFinder::DoesPathExist(const IPlugin* InPlugin, const FString& InRelativePath)
{
    FString Unused;
    return PathFinder::DoesPathExist(InPlugin, InRelativePath, Unused);
}

bool PathFinder::DoesPathExist(const IPlugin* InPlugin, const FString& InRelativePath, FString& OutRelativePath)
{
    PathFinder::CreatePath(InPlugin, InRelativePath, OutRelativePath);
    return PathFinder::DoesPathExist(OutRelativePath);
}

void PathFinder::CreatePath(const IPlugin* InPlugin, const FString& InRelativePath, FString& OutRelativePath)
{
    OutRelativePath = InPlugin ? (InPlugin->GetContentDir() / InRelativePath) : (FPaths::ProjectContentDir() / InRelativePath);
}

bool PathFinder::CreatePathToFile(const IPlugin* InPlugin, const FString& InRelativePath, FString& OutRelativePath)
{
    PathFinder::CreatePath(InPlugin, InRelativePath, OutRelativePath);
    return IFileManager::Get().FileExists(*OutRelativePath);
}

bool PathFinder::CreatePathToFile(
    const IPlugin* InPlugin,
    const EPathType::Type InPathType,
    const FString& FileName,
    FString& OutRelativePath
)
{
    return PathFinder::CreatePathToFile(InPlugin, LexToString(InPathType) / FileName, OutRelativePath);
}

bool PathFinder::CreatePathToDirectory(const IPlugin* InPlugin, const FString& InRelativePath, FString& OutRelativePath)
{
    OutRelativePath = InPlugin->GetContentDir() / InRelativePath;
    return IFileManager::Get().DirectoryExists(*OutRelativePath);
}

bool PathFinder::CreatePathToDirectory(const IPlugin* InPlugin, const EPathType::Type InPathType, FString& OutRelativePath)
{
    return PathFinder::CreatePathToDirectory(InPlugin, LexToString(InPathType), OutRelativePath);
}

bool PathFinder::LoadBytesFromDisk(const IPlugin* InPlugin, const FString& InRelativePath, TArray<uint8>& OutBytes)
{
    FString Path;
    if (PathFinder::CreatePathToFile(InPlugin, InRelativePath, Path) == false)
    {
        LOG_ERROR(LogSystem, "Failed to create path to file: %s.", *Path)
        return false;
    }

    PathFinder::RelativeToAbsolutePath(Path);

    if (FFileHelper::LoadFileToArray(OutBytes, *Path))
    {
        return true;
    }

    LOG_ERROR(LogSystem, "Failed to load file: %s.", *Path)

    return false;
}

bool PathFinder::LoadBytesFromDisk(const IPlugin* InPlugin, const EPathType::Type InPathType, const FString& InFileName, TArray<uint8>& OutBytes)
{
    return PathFinder::LoadBytesFromDisk(InPlugin, LexToString(InPathType) / InFileName, OutBytes);
}

FString PathFinder::LoadStringFromDisk(const IPlugin* InPlugin, const FString& InRelativePath)
{
    FString Path = InPlugin->GetContentDir() / InRelativePath;
    PathFinder::NormalizePath(Path);

    if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*Path) == false)
    {
        LOG_ERROR(LogSystem, "File does not exist: %s.", *Path)
        return TEXT("");
    }

    if (FString Content; FFileHelper::LoadFileToString(Content, *Path))
    {
        return Content;
    }

    LOG_ERROR(LogSystem, "Failed to load file: %s.", *Path)
    return TEXT("");
}

FString PathFinder::LoadStringFromDisk(const IPlugin* InPlugin, const EPathType::Type InPathType, const FString& InFileName)
{
    return PathFinder::LoadStringFromDisk(InPlugin, LexToString(InPathType) / InFileName);
}

TSharedPtr<FJsonObject> PathFinder::LoadJSONFromDisk(const IPlugin* InPlugin, const FString& InRelativePath)
{
    const FString Content = PathFinder::LoadStringFromDisk(InPlugin, InRelativePath);
    if (Content.IsEmpty())
    {
        return nullptr;
    }

    TSharedPtr<FJsonObject> JsonObject;

    if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Content), JsonObject) == false || JsonObject.IsValid() == false)
    {
        LOG_ERROR(LogSystem, "Failed to deserialize JSON from file: %s.", *InRelativePath)
        return nullptr;
    }

    return JsonObject;
}

TSharedPtr<FJsonObject> PathFinder::LoadJSONFromDisk(const IPlugin* InPlugin, const EPathType::Type InPathType, const FString& InFileName)
{
    FString FileName = InFileName;
    if (FileName.EndsWith(TEXT(".json")) == false)
    {
        FileName += TEXT(".json");
    }
    return PathFinder::LoadJSONFromDisk(InPlugin, LexToString(InPathType) / FileName);
}

TArray<FString> PathFinder::GetFilesInDirectory(
    const IPlugin*        InPlugin,
    const EPathType::Type InPathType,
    const FString&        InExtension /* = TEXT("") */,
    const bool            bRemoveExtension /* = true */
)
{
    FString Path;
    PathFinder::CreatePathToDirectory(InPlugin, InPathType, Path);
    PathFinder::RelativeToAbsolutePath(Path);

    if (PathFinder::DoesPathExist(Path) == false)
    {
        return TArray<FString>();
    }

    TArray<FString> Files;
    IFileManager::Get().FindFiles(Files, *Path, *InExtension);

    if (bRemoveExtension)
    {
        for (FString& File : Files)
        {
            if (InExtension.StartsWith("."))
            {
                File.RemoveFromEnd(InExtension);
            }
            else
            {
                File.RemoveFromEnd(TEXT(".") + InExtension);
            }
        }
    }

    return Files;
}
