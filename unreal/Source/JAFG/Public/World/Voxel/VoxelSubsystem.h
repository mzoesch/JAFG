// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VoxelMask.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "VoxelSubsystem.generated.h"

class UMaterialSubsystem;

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
	
	FORCEINLINE auto GetCommonVoxelNum(void) const -> int32 { return this->CommonVoxelNum; }
	FORCEINLINE auto GetTextureIndex(const int32 Voxel, const FVector& Normal) const -> int32 { return this->VoxelMasks[Voxel].GetTextureIndex(Normal); }
	FORCEINLINE auto GetTextureGroup(const int32 Voxel, const FVector& Vector) const -> ETextureGroup::Type { return this->VoxelMasks[Voxel].GetTextureGroup(Vector); }
};
