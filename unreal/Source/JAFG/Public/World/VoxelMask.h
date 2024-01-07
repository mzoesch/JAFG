// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "VoxelMask.generated.h"

USTRUCT()
struct JAFG_API FVoxelMask
{
    GENERATED_BODY()

public:

    FVoxelMask() = default;
    FVoxelMask(const FString& NameSpace, const FString& Name, const bool bIsTranslucent, const int TextureGroup, class IVoxel* VoxelClass);
    
public:
    
    FString         NameSpace;
    FString         Name;
    bool            bIsTranslucent;
    int             TextureGroup;
    class IVoxel*   VoxelClass;
};
