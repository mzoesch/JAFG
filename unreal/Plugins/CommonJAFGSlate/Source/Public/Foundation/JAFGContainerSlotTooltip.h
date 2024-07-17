// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGContainerSlot.h"
#include "JAFGDirtyUserWidget.h"

#include "JAFGContainerSlotTooltip.generated.h"

class UJAFGTextBlock;

struct COMMONJAFGSLATE_API FSlotTooltipPassData : public FWidgetPassData
{
    explicit FSlotTooltipPassData(UJAFGContainerSlot* InSlot) : Slot(InSlot) { return; }

    UJAFGContainerSlot* Slot;
};

UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGContainerSlotTooltip : public UJAFGDirtyUserWidget
{
    GENERATED_BODY()

public:

    explicit UJAFGContainerSlotTooltip(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UJAFGUserWidget implementation
    virtual void PassDataToWidget(const FWidgetPassData& UncastedData) override;
    // ~UJAFGUserWidget implementation

    FORCEINLINE auto SetRepresentedSlot(const TObjectPtr<UJAFGContainerSlot>& InSlot) -> void
    {
        this->RepresentedSlot = InSlot;
    }

    UFUNCTION(BlueprintPure, Category = "JAFG|Widget|Container", meta = (AllowPrivateAccess = "true"))
    FString GetAccumulatedName( /* void */ ) const;

    UFUNCTION(BlueprintPure, Category = "JAFG|Widget|Container", meta = (AllowPrivateAccess = "true"))
    FString GetAccumulatedNamespace( /* void */ ) const;

    /* As string because blueprints do not support the primitive type that we use to determine accumulates. */
    UFUNCTION(BlueprintPure, Category = "JAFG|Widget|Container", meta = (AllowPrivateAccess = "true"))
    FString GetAccumulatedIndex( /* void */ ) const;

protected:

    // UJAFGDirtyUserWidget implementation
    virtual void OnRefresh(void) override;
    // ~UJAFGDirtyUserWidget implementation

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true", OptionalWidget))
    TObjectPtr<UJAFGTextBlock> TextBlock_AccumulatedName;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true", OptionalWidget))
    TObjectPtr<UJAFGTextBlock> TextBlock_AccumulatedDebugInfo;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true", OptionalWidget))
    TObjectPtr<UJAFGTextBlock> TextBlock_AccumulatedDebugName;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true", OptionalWidget))
    TObjectPtr<UJAFGTextBlock> TextBlock_AccumulatedIndex;

private:

    UPROPERTY()
    TObjectPtr<UJAFGContainerSlot> RepresentedSlot;
};
