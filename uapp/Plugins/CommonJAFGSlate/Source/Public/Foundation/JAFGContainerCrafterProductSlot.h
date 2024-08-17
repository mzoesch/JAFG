// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGContainerSlot.h"
#include "System/RecipeSubsystem.h"

#include "JAFGContainerCrafterProductSlot.generated.h"

class IContainerCrafter;
class UJAFGContainerCrafterProductSlot;

/** @note Only use for the OSD list entry passes and never to pass actual data around. */
UCLASS(NotBlueprintable)
class UJAFGContainerCrafterProductSlotData : public UJAFGContainerSlotData
{
    GENERATED_BODY()

    friend UJAFGContainerCrafterProductSlot;

public:

    auto GetPredictedOwner(void) const -> IContainerCrafter*;

    virtual auto GetSlot(void) const -> FSlot override;
    virtual auto GetSlotRef(void) const -> FSlot& override;
    virtual auto GetSlotValue(void) const -> FAccumulated override;
    virtual auto GetSlotValueRef(void) const -> FAccumulated& override;
    virtual auto GetSlotAmount(void) const -> accamount_t override;
    virtual auto GetSlotAmountRef(void) const -> accamount_t& override;

protected:

    FORCEINLINE void SetCachedRecipeProduct(const FRecipeProduct& InCachedRecipeProduct) const { this->CachedRecipeProduct = InCachedRecipeProduct; }
    FORCEINLINE void InvalidateCachedRecipeProduct(void) const { this->CachedRecipeProduct = ::RecipeProduct::Null; }

    mutable FRecipeProduct CachedRecipeProduct;
};

/** Optional widget to use as a foundation for container slots that are represented a container crafter. */
UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGContainerCrafterProductSlot : public UJAFGContainerSlot
{
    GENERATED_BODY()

public:

    explicit UJAFGContainerCrafterProductSlot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void SetCrafterTargetContainer(IContainerCrafter* TargetContainer);

protected:

    // UUserWidget implementation
    virtual auto NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> FReply override;
    // ~UUserWidget implementation

private:

    IContainerCrafter* CrafterTargetContainer = nullptr;
    FDelegateHandle OnContainerChangedDelegateHandle;

    UFUNCTION()
    void OnCrafterTargetContainerChanged(const ELocalContainerChange::Type InReason, const int32 InIndex);
};
