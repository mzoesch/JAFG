// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExternalSubsystem.h"
#include "JAFGTypeDefs.h"
#include "VoxelMask.h"
#include "ItemMask.h"

#include "VoxelSubsystem.generated.h"

class UMaterialSubsystem;

UCLASS(NotBlueprintable)
class JAFGEXTERNALCORE_API UVoxelSubsystem : public UExternalSubsystem
{
    GENERATED_BODY()

    friend UMaterialSubsystem;

public:

    UVoxelSubsystem();

    // Subsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto Deinitialize(void) -> void override;
    // ~Subsystem implementation

    FORCEINLINE auto GetCommonVoxelNum(void) const -> voxel_t { return this->CommonVoxelNum; }

    /** Also includes all common voxels. */
    FORCEINLINE auto GetVoxelNum(void) const -> voxel_t { return this->VoxelMasks.Num(); }
    /** The index where all other accumulates are items and not voxels anymore. */
    FORCEINLINE auto GetItemIndexStart(void) const -> voxel_t { return this->VoxelMasks.Num(); }

    FORCEINLINE auto GetVoxelName(const voxel_t Voxel) const -> FString { return this->VoxelMasks[Voxel].Name; }
                auto GetVoxelIndex(const FString& Name) const -> voxel_t;
                auto GetVoxelIndex(const FString& NameSpace, const FString& Name) const -> voxel_t;
    FORCEINLINE auto GetVoxelNamespace(const voxel_t Voxel) const -> FString { return this->VoxelMasks[Voxel].Namespace; }
    FORCEINLINE auto GetVoxelTextureGroup(const voxel_t Voxel, const FVector& Normal) const -> ETextureGroup::Type { return this->VoxelMasks[Voxel].GetTextureGroup(Normal); }
    FORCEINLINE auto GetTextureIndex(const voxel_t Voxel, const FVector& Normal) const -> int32 { return this->VoxelMasks[Voxel].GetTextureIndex(Normal); };

    FORCEINLINE auto GetItemName(const voxel_t Item) const -> FString { return this->ItemMasks[this->TransformAccumulatedToItem(Item)].Name; }
                auto GetItemIndex(const FString& Name) const -> voxel_t;
                auto GetItemIndex(const FString& NameSpace, const FString& Name) const -> voxel_t;
    FORCEINLINE auto GetItemNamespace(const voxel_t Item) const -> FString { return this->ItemMasks[this->TransformAccumulatedToItem(Item)].Namespace; }

    FORCEINLINE auto GetAccumulatedName(const voxel_t Accumulated) const -> FString { return this->VoxelMasks.IsValidIndex(Accumulated) ? this->GetVoxelName(Accumulated) : this->GetItemName(Accumulated); }
                auto GetAccumulatedIndex(const FString& Name) const -> voxel_t;
                auto GetAccumulatedIndex(const FString& NameSpace, const FString& Name) const -> voxel_t;
    FORCEINLINE auto GetAccumulatedNamespace(const voxel_t Accumulated) const -> FString { return this->VoxelMasks.IsValidIndex(Accumulated) ? this->GetVoxelNamespace(Accumulated) : this->GetItemNamespace(Accumulated); }

private:

    UPROPERTY()
    TArray<FVoxelMask> VoxelMasks;
    /**
     * Wrapped into a variable and not a method to safe some run-time.
     * -1 means not initialized.
     */
    int32 CommonVoxelNum = -1;
    /** Has to be called after initialization. */
    void SetCommonVoxelNum(void);

    void InitializeCommonVoxels(void);
    void InitializeOptionalVoxels(void);

    UPROPERTY()
    TArray<FItemMask> ItemMasks;
    void InitializeOptionalItems(void);

    FORCEINLINE voxel_t TransformAccumulatedToItem(const voxel_t Accumulated) const
    {
        return Accumulated - this->GetItemIndexStart();
    }
};
