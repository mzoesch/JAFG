// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "VoxelMask.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "System/MaterialSubsystem.h"

#include "VoxelSubsystem.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API UVoxelSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    friend UMaterialSubsystem;

public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;

private:

    UPROPERTY()
    TArray<FVoxelMask> VoxelMasks;
    /** Wrapped into a variable and not a method to safe some run-time. */
    int32 CommonVoxelNum = -1;
    /** Has to be called after initialization. */
    void SetCommonVoxelNum(void);

    void InitializeCommonVoxels(void);
    void InitializeOptionalVoxels(void);

public:

    FORCEINLINE auto GetCommonVoxelNum(void) const -> int32
    {
        return this->CommonVoxelNum;
    }

    /** The index where all other accumulates are items and not voxels anymore. */
    FORCEINLINE auto GetItemIndexStart(void) const -> int32
    {
        return this->VoxelMasks.Num();
    }

    /** Also includes all common voxels. */
    FORCEINLINE auto GetVoxelNum(void) const -> int32
    {
        return this->VoxelMasks.Num();
    }

    FORCEINLINE auto GetVoxelName(const int32 Voxel) const -> FString
    {
#if WITH_EDITOR
        if (this->VoxelMasks.Num() <= Voxel)
        {
            LOG_FATAL(LogTemp, "Voxel index [%d] out of bounds", Voxel)
            return FString();
        }
#endif /* WITH_EDITOR */

        return this->VoxelMasks[Voxel].Name;
    }

    FORCEINLINE auto GetTextureIndex(const int32 Voxel, const FVector& Normal) const -> int32
    {
#if WITH_EDITOR
        if (this->VoxelMasks.Num() <= Voxel)
        {
            LOG_FATAL(LogTemp, "Voxel index [%d] out of bounds", Voxel)
            return 0;
        }
#endif /* WITH_EDITOR */

        return this->VoxelMasks[Voxel].GetTextureIndex(Normal);
    }

    FORCEINLINE auto GetTextureGroup(const int32 Voxel, const FVector& Vector) const -> ETextureGroup::Type
    {
#if WITH_EDITOR
        if (this->VoxelMasks.Num() <= Voxel)
        {
            LOG_FATAL(LogTemp, "Voxel index [%d] out of bounds", Voxel)
            return ETextureGroup::Opaque;
        }
#endif /* WITH_EDITOR */

        return this->VoxelMasks[Voxel].GetTextureGroup(Vector);
    }
};
