// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/Container/Container.h"
#include "HUD/Container/Slots/ContainerSlot.h"
#include "Lib/PrescriptionSeeker.h"

#include "World/Voxels/Voxel.h"

#include "CraftingTableVoxel.generated.h"

struct FSlot;
struct FAccumulated;
class UGI_Master;
class ACH_Master;

UCLASS()
class JAFG_API UW_CraftingTableCrafterSlot : public UW_ContainerSlot
{
    GENERATED_BODY()

public:

    virtual void OnClicked() override;
    virtual void OnSecondaryClicked() override;
};

UCLASS()
class JAFG_API UW_CraftingTableCrafterProduct : public UW_ContainerSlot
{
    GENERATED_BODY()

public:

    virtual void OnClicked() override;
};

UCLASS()
class JAFG_API UW_CraftingTableContainer : public UW_Container
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTileView* CraftingTableCrafterSlots;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UW_CraftingTableCrafterProduct* CraftingTableCrafterProduct;
    
protected:

    virtual void NativeOnInitialized() override;
    virtual void OnDestroy() override;
    virtual void NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) override;
    virtual void RefreshCharacterInventory() override;

private:

    TArray<FSlot> CrafterSlots;

    FDelivery GetCrafterAsDelivery(void);
    FAccumulated GetCrafterProduct(void);
    /** The caller has the responsibility to update the HUD accordingly. */
    void ReduceCrafterByProductIngredients(void);
    
    friend void UW_CraftingTableCrafterSlot::OnClicked(void);
    friend void UW_CraftingTableCrafterSlot::OnSecondaryClicked(void);
    friend void UW_CraftingTableCrafterProduct::OnClicked(void);
};

UCLASS()
class JAFG_API UCraftingTableVoxel : public UObject, public IVoxel
{
    GENERATED_BODY()
    
public:

    virtual void Initialize(UGI_Master* InGIPtr) override;
    virtual void OnCustomSecondaryCharacterEvent(ACH_Master* Caller, bool& bConsumed) override;

public:

    static constexpr int CrafterWidth       = 3;
    static constexpr int CrafterSlotCount   = 9;
    
private:

    TObjectPtr<UGI_Master> GIPtr;
};
