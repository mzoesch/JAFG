// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Voxel.h"

#include "OakLogVoxel.generated.h"

class UGI_Master;
class ACH_Master;

UCLASS()
class JAFG_API UOakLogVoxel : public UObject, public IVoxel
{
    GENERATED_BODY()
    
public:

    virtual void Initialize(UGI_Master* GIPtr) override;
    virtual void OnCustomSecondaryCharacterEvent(ACH_Master* Caller, bool& bConsumed) override;
};
