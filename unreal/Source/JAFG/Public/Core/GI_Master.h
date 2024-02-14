// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Lib/FAccumulated.h"

#include "World/VoxelMask.h"
#include "World/ItemMask.h"

#include "GI_Master.generated.h"

class ACuboid;

UCLASS()
class JAFG_API UGI_Master : public UGameInstance
{
    GENERATED_BODY()

public:

    virtual void Init() override;

private:

    /** The Name Space must be in this order: CORE, JAFG, ... */
    TArray<FVoxelMask> VoxelMasks;
    TArray<FItemMask> ItemMasks;
    
public:

    FORCEINLINE void AddVoxelMask(const FVoxelMask& VoxelMask) { this->VoxelMasks.Add(VoxelMask); }
    
private:
    
    void InitializeVoxels();
    void InitializeItems();
    void InitializeVoxelMaterials();
    void InitializeMaterials();

public:
    
    FORCEINLINE bool IsVoxelTranslucent(const int Voxel) const { return this->VoxelMasks[Voxel].bIsTranslucent; }
    FORCEINLINE int GetTextureIndex(const int Voxel, const FVector& Normal) const { return this->VoxelMasks[Voxel].GetTextureIndex(Normal); }
    FORCEINLINE int GetTextureGroup(const int Voxel) const { return this->VoxelMasks[Voxel].TextureGroup; }
    FORCEINLINE FString GetVoxelName(const int Voxel) const
    {
        if (FAccumulated(Voxel).IsVoxel())
        {
            return this->VoxelMasks[Voxel].Name;
        }
        return this->ItemMasks[Voxel - this->GetItemIndexStart()].Name;
    }
    FORCEINLINE int GetVoxelNum() const { return this->VoxelMasks.Num(); }
    int GetCoreVoxelNum() const;
    FAccumulated GetAccumulatedByName(const FString& Name) const;

    /** The index where all other accumulates are items and not voxels anymore. */
    FORCEINLINE int GetItemIndexStart() const { return this->VoxelMasks.Num(); }
    
    UPROPERTY(EditDefaultsOnly, Category="Texture")
    UTexture2D* NoTexture;

#pragma region Materials, Textures
    
public:

    UPROPERTY(EditDefaultsOnly, Category="Material")
    const TObjectPtr<UMaterialInterface> MOpaque;

    UPROPERTY(VisibleDefaultsOnly, Category="Material")
    TObjectPtr<UMaterialInstanceDynamic> MDynamicOpaque;

    UPROPERTY(EditDefaultsOnly, Category="Material")
    const TObjectPtr<UMaterialInterface> MItem;
    
#pragma endregion Materials, Textures
    
};
