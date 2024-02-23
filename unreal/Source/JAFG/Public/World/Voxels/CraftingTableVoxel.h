// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Voxel.h"

#include "CraftingTableVoxel.generated.h"

class UGI_Master;

UCLASS()
class JAFG_API UCraftingTableVoxel : public UObject, public IVoxel
{
    GENERATED_BODY()
    
public:

    virtual void Initialize(UGI_Master* GIPtr) override;
};
