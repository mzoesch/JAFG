// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGGameInstanceSubsystem.h"

#include "VoxelSubsystem.generated.h"

JAFG_VOID

class UMaterialSubsystem;

UCLASS(NotBlueprintable)
class JAFG_API UVoxelSubsystem : public UJAFGGameInstanceSubsystem
{
    GENERATED_BODY()

    friend UMaterialSubsystem;

public:

    UVoxelSubsystem();

    // Subsystem implementation
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto Deinitialize(void) -> void override;
    // ~Subsystem implementation

    FORCEINLINE auto AddOptionalVoxelDuringInitialization(const FVoxelMask& VoxelMask) -> void { this->VoxelMasks.Add(VoxelMask); }
    FORCEINLINE auto GetCommonVoxelNum(void) const -> voxel_t { return this->CommonVoxelNum; }
    /** The index where all other accumulates are items and not voxels anymore. */
    FORCEINLINE auto GetItemIndexStart(void) const -> voxel_t { return this->VoxelMasks.Num(); }
    /** Also includes all common voxels. */
    FORCEINLINE auto GetVoxelNum(void) const -> voxel_t { return this->VoxelMasks.Num(); }
    FORCEINLINE auto GetVoxelName(const voxel_t Voxel) const -> FString { return this->VoxelMasks[Voxel].Name; }
                auto GetVoxelIndex(const FString& NameSpace, const FString& Name) const -> voxel_t;
    FORCEINLINE auto GetVoxelNamespace(const voxel_t Voxel) const -> FString { return this->VoxelMasks[Voxel].NameSpace; }
    FORCEINLINE auto GetVoxelTextureGroup(const voxel_t Voxel, const FVector& Normal) const -> ETextureGroup::Type { return this->VoxelMasks[Voxel].GetTextureGroup(Normal); }
    FORCEINLINE auto GetTextureIndex(const voxel_t Voxel, const FVector& Normal) const -> int32 { return this->VoxelMasks[Voxel].GetTextureIndex(Normal); };

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
};
