// © 2023 mzoesch. All rights reserved.

#pragma once

#include "Lib/FAccumulated.h"

#include "General.generated.h"

USTRUCT()
struct JAFG_API FGeneral
{
    GENERATED_BODY()

public:
    
    static const inline FString GeneratedAssetsDirectoryRelative = FPaths::ProjectSavedDir() + "gen/";
    static const inline FString GeneratedAssetsDirectory = FPaths::ConvertRelativePathToFull(FGeneral::GeneratedAssetsDirectoryRelative);
    

private:

    static UTexture2D* LoadTexture2DFromDisk(const FString& Path);
    
public:
    
    static UTexture2D* LoadTexture2D(const FAccumulated Accumulated);
    
};

