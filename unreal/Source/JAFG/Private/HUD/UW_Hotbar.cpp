// © 2023 mzoesch. All rights reserved.

#include "HUD/UW_Hotbar.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "HUD/UW_HotbarSlot.h"

void UUW_Hotbar::InitializeHotbar(const TSubclassOf<UUW_HotbarSlot> UWHotbarSlotClass) const
{
    this->UWHotbarSlots->ClearChildren();
    UCanvasPanelSlot* CanvasSlotWContainer = CastChecked<UCanvasPanelSlot>(this->UWHotbarSlotsContainer->Slot);
    CanvasSlotWContainer->SetSize({FVector2D(this->SlotCount * this->SlotSizeX, this->SlotSizeY)});
    CanvasSlotWContainer->SetPosition({FVector2D(-(this->SlotCount * this->SlotSizeX) / 2, -(this->SlotSizeY + UUW_Hotbar::BottomMargin))});
    
    for (int i = 0; i < this->SlotCount; i++)
    {
        UUW_HotbarSlot* HotbarSlot = CreateWidget<UUW_HotbarSlot>(this->GetWorld(), UWHotbarSlotClass);
        UCanvasPanelSlot* CanvasSlot = this->UWHotbarSlots->AddChildToCanvas(HotbarSlot);
        CanvasSlot->SetSize({FVector2D(this->SlotSizeX, this->SlotSizeY)});
        CanvasSlot->SetPosition({static_cast<double>(i * this->SlotSizeX), 0.0f});
        continue;
    }

    return;
}

void UUW_Hotbar::OnSlotSelect()
{
}
