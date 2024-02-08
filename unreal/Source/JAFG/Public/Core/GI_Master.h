// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Lib/FAccumulated.h"

#include "World/VoxelMask.h"

#include "GI_Master.generated.h"

class ACuboid;

UCLASS()
class JAFG_API UGI_Master : public UGameInstance
{
    GENERATED_BODY()

public:

    virtual void Init() override;

private:

    TArray<FVoxelMask> VoxelMasks;
    // TODO Here another array for the items?
    
public:

    void AddVoxelMask(const FVoxelMask& VoxelMask);
    
private:
    
    void InitializeVoxels();
    void InitializeMaterials();

public:

    bool IsVoxelTranslucent(const int Voxel) const;
    int GetTextureIndex(const int Voxel, const FVector& Normal) const;
    int GetTextureGroup(const int Voxel) const;
    FString GetVoxelName(const int Voxel) const;
    int GetVoxelNum() const;
    int GetCoreVoxelNum() const;
    FAccumulated GetAccumulatedByName(const FString& Name) const;
    
    UPROPERTY(EditDefaultsOnly, Category="Texture")
    UTexture2D* NoTexture;

#pragma region Materials
    
public:

    UPROPERTY(EditDefaultsOnly, Category="Material")
    const TObjectPtr<UMaterialInterface> MOpaque;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> MDynamicOpaque;

#pragma endregion Materials
    
};
