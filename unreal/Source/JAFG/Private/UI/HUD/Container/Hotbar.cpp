// Copyright 2024 mzoesch. All rights reserved.

#include "UI/HUD/Container/Hotbar.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "UI/HUD/Container/HotbarSelector.h"
#include "UI/HUD/Container/Slots/HotbarSlot.h"

void UHotbar::InitializeHotbar(const TSubclassOf<UHotbarSlot> HotbarSlotClass, const TSubclassOf<UHotbarSelector> HotbarSelectorClass)
{
    check( this->B_SlotContainer )
    check( this->CP_Slots )

    this->CP_Slots->ClearChildren();

    for (int i = 0; i < 10; ++i)
    {
        UHotbarSlot* HotbarSlot = CreateWidget<UHotbarSlot>(this->GetWorld(), HotbarSlotClass);
        check( HotbarSlot )

        HotbarSlot->RenderSlot();

        UCanvasPanelSlot* CanvasPanelSlot = this->CP_Slots->AddChildToCanvas(HotbarSlot);
        check( CanvasPanelSlot )
        CanvasPanelSlot->SetSize(FVector2D(this->SlotSizeX, this->SlotSizeY));
        CanvasPanelSlot->SetPosition(FVector2D(i * this->SlotSizeX, 0.0f));

        continue;
    }

    UHotbarSelector* HotbarSelector = CreateWidget<UHotbarSelector>(this->GetWorld(), HotbarSelectorClass);
    check( HotbarSelector )
    this->CPS_Selector = this->CP_Slots->AddChildToCanvas(HotbarSelector);
    check( this->CPS_Selector )
    this->CPS_Selector->SetSize(FVector2D(this->SlotSizeX + this->HotbarSelectorOverlap, this->SlotSizeY + this->HotbarSelectorOverlap));


    return;
}

void UHotbar::RefreshSelectorLocation(void) const
{
    this->CPS_Selector->SetPosition(FVector2D(0.0f, 0.0f));
}

void UHotbar::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->MarkAsDirty();

    return;
}

void UHotbar::OnRefresh(void) const
{
    check( CP_Slots )

    this->RefreshSelectorLocation();

    return;
}
