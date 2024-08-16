// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Foundation/JAFGContainerSlot.h"

#include "RecipeContainerSlot.generated.h"

JAFG_VOID

class UAccumulatedRecipeSummary;

UCLASS()
class JAFG_API URecipeContainerSlotData : public UJAFGReadOnlyContainerSlotData
{
    GENERATED_BODY()

public:

    UPROPERTY()
    TObjectPtr<UAccumulatedRecipeSummary> Owner;
};

DECLARE_DELEGATE_RetVal_OneParam(bool /* bHandled */, FOnRecipeContainerSlotClicked, const FAccumulated& /* Accumulated */)

/**
 * A slot that shows possible recipes when clicked.
 * @note Uses UJAFGReadOnlyContainerSlotData as inherited data.
 */
UCLASS(Abstract, Blueprintable)
class JAFG_API URecipeContainerSlot : public UJAFGReadOnlyContainerSlot
{
    GENERATED_BODY()

public:

    explicit URecipeContainerSlot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    FOnRecipeContainerSlotClicked OnRecipeContainerSlotClicked;

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> FReply override;
    // ~UUserWidget implementation
};
