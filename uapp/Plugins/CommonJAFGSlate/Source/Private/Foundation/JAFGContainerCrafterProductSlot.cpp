// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/JAFGContainerCrafterProductSlot.h"
#include "ContainerCrafter.h"

typedef UJAFGContainerCrafterProductSlotData UThisSlotData;

#define GENERATED_SLOT_DATA_STUB()                                                           \
    if (this->CachedRecipeProduct.IsNull())                                                  \
    {                                                                                        \
        this->CachedRecipeProduct = this->GetPredictedOwner()->GetContainerCrafterProduct(); \
    }

IContainerCrafter* UJAFGContainerCrafterProductSlotData::GetPredictedOwner(void) const
{
    return Cast<IContainerCrafter>(this->Owner);
}

FSlot UJAFGContainerCrafterProductSlotData::GetSlot(void) const
{
    GENERATED_SLOT_DATA_STUB()
    return FSlot(this->CachedRecipeProduct.Product);
}

FSlot& UJAFGContainerCrafterProductSlotData::GetSlotRef(void) const
{
    jcheckNoEntry()
    return Super::GetSlotRef();
}

FAccumulated UJAFGContainerCrafterProductSlotData::GetSlotValue(void) const
{
    GENERATED_SLOT_DATA_STUB()
    return this->CachedRecipeProduct.Product;
}

FAccumulated& UJAFGContainerCrafterProductSlotData::GetSlotValueRef(void) const
{
    GENERATED_SLOT_DATA_STUB()
    return this->CachedRecipeProduct.Product;
}

accamount_t UJAFGContainerCrafterProductSlotData::GetSlotAmount(void) const
{
    GENERATED_SLOT_DATA_STUB()
    return this->CachedRecipeProduct.Product.Amount;
}

accamount_t& UJAFGContainerCrafterProductSlotData::GetSlotAmountRef(void) const
{
    GENERATED_SLOT_DATA_STUB()
    return this->CachedRecipeProduct.Product.Amount;
}

#undef GENERATED_SLOT_DATA_STUB

UJAFGContainerCrafterProductSlot::UJAFGContainerCrafterProductSlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGContainerCrafterProductSlot::SetCrafterTargetContainer(IContainerCrafter* TargetContainer)
{
    if (this->CrafterTargetContainer && this->OnContainerChangedDelegateHandle.IsValid())
    {
        if (this->CrafterTargetContainer->OnContainerChangedDelegate.Remove(this->OnContainerChangedDelegateHandle) == false)
        {
            LOG_ERROR(LogCommonSlate, "Failed to remove container changed delegate.")
        }
    }

    this->CrafterTargetContainer = TargetContainer;
    this->CrafterTargetContainer->OnContainerChangedDelegate.AddUObject(this, &UJAFGContainerCrafterProductSlot::OnCrafterTargetContainerChanged);

    UJAFGContainerCrafterProductSlotData* CrafterSlotData = NewObject<UJAFGContainerCrafterProductSlotData>(this);
    /* If somehow we use the index, we should crash. */
    CrafterSlotData->Index = INDEX_NONE;
    CrafterSlotData->Owner = this->CrafterTargetContainer;

    this->SetSlotData(CrafterSlotData);

    return;
}

FReply UJAFGContainerCrafterProductSlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    LOG_WARNING(LogTemp, "Called. Not implementec yet.")
    return FReply::Handled();
}

void UJAFGContainerCrafterProductSlot::OnCrafterTargetContainerChanged(const ELocalContainerChange::Type InReason, const int32 InIndex)
{
    this->GetSlotData<UThisSlotData>().InvalidateCachedRecipeProduct();
    this->RenderSlot();

    return;
}
