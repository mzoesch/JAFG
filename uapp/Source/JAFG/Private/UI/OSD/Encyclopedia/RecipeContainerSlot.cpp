// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/Encyclopedia/RecipeContainerSlot.h"
#include "JAFGSlateSettings.h"
#include "UI/OSD/Encyclopedia/AccumulatedRecipeSummary.h"

URecipeContainerSlot::URecipeContainerSlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void URecipeContainerSlot::NativeConstruct(void)
{
    Super::NativeConstruct();
}

FReply URecipeContainerSlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (this->OnRecipeContainerSlotClicked.IsBound())
    {
        if (this->OnRecipeContainerSlotClicked.Execute(this->GetUncastedSlotData()->GetSlotValue()))
        {
            return FReply::Handled();
        }
    }

    const FAccumulated Value = this->GetUncastedSlotData()->GetSlotValue();
    if (Value.IsNull() || Value == ECommonVoxels::Air)
    {
        return FReply::Handled();
    }

    UJAFGUserWidget* Widget = CreateWidget<UJAFGUserWidget>(this->GetWorld(), GetDefault<UJAFGSlateSettings>()->AccumulatedRecipeSummaryWidgetClass);
    Widget->PassDataToWidget(FAccumulatedRecipeSummaryPassData(Value));
    Widget->AddToViewport();

    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}
