// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/Container/Container.h"

#include "World/Voxels/Voxel.h"

#include "CraftingTableVoxel.generated.h"

class UGI_Master;
class ACH_Master;

UCLASS()
class JAFG_API UW_CraftingTableContainer : public UW_Container
{
    GENERATED_BODY()

protected:

    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    virtual void RefreshCharacterInventory() override;
};

UCLASS()
class JAFG_API UCraftingTableVoxel : public UObject, public IVoxel
{
    GENERATED_BODY()
    
public:

    virtual void Initialize(UGI_Master* InGIPtr) override;
    virtual void OnCustomSecondaryCharacterEvent(ACH_Master* Caller, bool& bConsumed) override;

private:

    TObjectPtr<UGI_Master> GIPtr;
};
