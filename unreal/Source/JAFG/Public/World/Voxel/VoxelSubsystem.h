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

	FORCEINLINE int32 GetCommonVoxelNum(void) const { return this->CommonVoxelNum; }
};
