// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Voxels/NormalLookup.h"

#include "VoxelMask.generated.h"

class IVoxel;

USTRUCT()
struct JAFG_API FVoxelMask
{
    GENERATED_BODY()

public:

    FVoxelMask() = default;
    FVoxelMask(const FString& NameSpace, const FString& Name, const int TextureGroup, TScriptInterface<IVoxel>* VoxelClass);
    FVoxelMask(const FString& NameSpace, const FString& Name, const TMap<ENormalLookup, int>& TextureGroup, TScriptInterface<IVoxel>* VoxelClass);

public:
    /**
     * This 
     * 
     */
    static const FVoxelMask NullVoxelMask;
    
public:
    
    FString                     NameSpace;
    FString                     Name;
    TMap<ENormalLookup, int>    TextureGroup;
    UPROPERTY()
    TScriptInterface<IVoxel>    VoxelClass;
    TMap<ENormalLookup, int>    NormalLookup;

public:
    
    int GetTextureIndex(const FVector& Normal) const;
    int GetTextureGroup(const FVector& Normal) const;
    FORCEINLINE FString ToString() const { return FString::Printf(TEXT("{Name:%s::%s, bVoxelClass:%hhd}"), *this->NameSpace, *this->Name, this->VoxelClass != nullptr); }
};
