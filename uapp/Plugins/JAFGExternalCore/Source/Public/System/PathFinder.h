// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"

class IPlugin;

/** Registered path types that JAFG can automatically detect. */
namespace EPathType
{

enum JAFGEXTERNALCORE_API Type : uint8
{
    //////////////////////////////////////////////////////////////////////////
    // Inside Textures directory
    Destruction,
    GUI,
    Items,
    Voxels,
    VoxelsAlpha,

    //////////////////////////////////////////////////////////////////////////
    // Top level directories
    Recipes,
    Fonts,
};

}

auto JAFGEXTERNALCORE_API LexToString(const EPathType::Type InType) -> FString;

/**
 * If a function returns a boolean, then the return value is true if the path to the file or directory exists.
 */
namespace PathFinder
{

auto JAFGEXTERNALCORE_API RelativeToAbsolutePath(FString& Path) -> void;
auto JAFGEXTERNALCORE_API RelativeToAbsolutePath(const IPlugin* InPlugin, const FString& InRelativePath, FString& OutAbsolutePath) -> void;
auto JAFGEXTERNALCORE_API RelativeToAbsolutePath(const IPlugin* InPlugin, const EPathType::Type InPathType, const FString& InFileName, FString& OutAbsolutePath) -> void;

auto JAFGEXTERNALCORE_API NormalizePath(FString& Path) -> void;

auto JAFGEXTERNALCORE_API DoesPathExist(const FString& Path) -> bool;
auto JAFGEXTERNALCORE_API DoesPathExist(const IPlugin* InPlugin, const FString& InRelativePath) -> bool;
auto JAFGEXTERNALCORE_API DoesPathExist(const IPlugin* InPlugin, const FString& InRelativePath, FString& OutRelativePath) -> bool;

auto JAFGEXTERNALCORE_API CreatePath(const IPlugin* InPlugin, const FString& InRelativePath, FString& OutRelativePath) -> void;
auto JAFGEXTERNALCORE_API CreatePathToFile(const IPlugin* InPlugin, const FString& InRelativePath, FString& OutRelativePath) -> bool;
auto JAFGEXTERNALCORE_API CreatePathToFile(const IPlugin* InPlugin, const EPathType::Type InPathType, const FString& FileName, FString& OutRelativePath) -> bool;
auto JAFGEXTERNALCORE_API CreatePathToDirectory(const IPlugin* InPlugin, const FString& InRelativePath, FString& OutRelativePath) -> bool;
auto JAFGEXTERNALCORE_API CreatePathToDirectory(const IPlugin* InPlugin, const EPathType::Type InPathType, FString& OutRelativePath) -> bool;

auto JAFGEXTERNALCORE_API LoadBytesFromDisk(const IPlugin* InPlugin, const FString& InRelativePath, TArray<uint8>& OutBytes) -> bool;
auto JAFGEXTERNALCORE_API LoadBytesFromDisk(const IPlugin* InPlugin, const EPathType::Type InPathType, const FString& InFileName, TArray<uint8>& OutBytes) -> bool;
auto JAFGEXTERNALCORE_API LoadStringFromDisk(const IPlugin* InPlugin, const FString& InRelativePath) -> FString;
auto JAFGEXTERNALCORE_API LoadStringFromDisk(const IPlugin* InPlugin, const EPathType::Type InPathType, const FString& InFileName) -> FString;
auto JAFGEXTERNALCORE_API LoadJSONFromDisk(const IPlugin* InPlugin, const FString& InRelativePath) -> TSharedPtr<FJsonObject>;
auto JAFGEXTERNALCORE_API LoadJSONFromDisk(const IPlugin* InPlugin, const EPathType::Type InPathType, const FString& InFileName) -> TSharedPtr<FJsonObject>;

/**
 * @param InExtension If FileExtension is an empty string, then all files are found. Otherwise, only files with the
 *                    specified extension are found. Can be the form of ".ext" or "ext".
 */
auto JAFGEXTERNALCORE_API GetFilesInDirectory(
    const IPlugin* InPlugin,
    const EPathType::Type InPathType,
    const FString& InExtension = TEXT(""),
    const bool bRemoveExtension = true
) -> TArray<FString>;

}
