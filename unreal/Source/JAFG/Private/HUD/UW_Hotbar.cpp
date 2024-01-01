// © 2023 mzoesch. All rights reserved.

#include "HUD/UW_Hotbar.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "HUD/UW_HotbarSlot.h"
#include "Core/CH_Master.h"
#include "HUD/UW_HotbarSelector.h"

void UUW_Hotbar::InitializeHotbar(const TSubclassOf<UUW_HotbarSlot> UWHotbarSlotClass, const TSubclassOf<UUW_HotbarSelector> UWHotbarSelectorClass)
{
    this->UWHotbarSlots->ClearChildren();
    UCanvasPanelSlot* CanvasSlotWContainer = CastChecked<UCanvasPanelSlot>(this->UWHotbarSlotsContainer->Slot);
    CanvasSlotWContainer->SetSize({FVector2D(this->SlotCount * this->SlotSizeX, this->SlotSizeY)});
    CanvasSlotWContainer->SetPosition({FVector2D(-(this->SlotCount * this->SlotSizeX) / 2, -(this->SlotSizeY + UUW_Hotbar::BottomMargin))});
    
    for (int i = 0; i < this->SlotCount; i++)
    {
        UUW_HotbarSlot* HotbarSlot = CreateWidget<UUW_HotbarSlot>(this->GetWorld(), UWHotbarSlotClass);
        HotbarSlot->OnItemPreviewUpdate(CastChecked<ACH_Master>(this->GetOwningPlayer()->GetCharacter())->GetInventorSlot(i));

        UCanvasPanelSlot* CanvasSlot = this->UWHotbarSlots->AddChildToCanvas(HotbarSlot);
        CanvasSlot->SetSize({FVector2D(this->SlotSizeX, this->SlotSizeY)});
        CanvasSlot->SetPosition({static_cast<double>(i * this->SlotSizeX), 0.0f});

        continue;
    }

    UUW_HotbarSelector* UWSelector = CreateWidget<UUW_HotbarSelector>(this->GetWorld(), UWHotbarSelectorClass);
    this->Selector = this->UWHotbarSlots->AddChildToCanvas(UWSelector);
    this->Selector->SetSize({FVector2D(this->SlotSizeX, this->SlotSizeY)});
    this->OnSlotSelect();
    
    return;
}

void UUW_Hotbar::OnSlotSelect() const
{
    this->Selector->SetPosition({static_cast<double>(CastChecked<ACH_Master>(this->GetOwningPlayer()->GetCharacter())->QuickSlotSelected * this->SlotSizeX), 0.0f});
    return;
}

void UUW_Hotbar::OnHotbarUpdate()
{
    for (int i = 0; i < this->SlotCount; i++)
    {
        const UUW_HotbarSlot* HotbarSlot = CastChecked<UUW_HotbarSlot>(this->UWHotbarSlots->GetChildAt(i));
        HotbarSlot->OnItemPreviewUpdate(CastChecked<ACH_Master>(this->GetOwningPlayer()->GetCharacter())->GetInventorSlot(i));
        continue;
    }
    
    return;
}
