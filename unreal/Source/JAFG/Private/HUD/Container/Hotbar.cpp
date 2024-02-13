// © 2023 mzoesch. All rights reserved.

#include "HUD/Container/Hotbar.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "HUD/Container/Slots/HotbarSlot.h"
#include "Core/CH_Master.h"
#include "HUD/Container/HotbarSelector.h"
#include "Lib/HUD/Container/ContainerSlotData.h"

void UW_Hotbar::InitializeHotbar(const TSubclassOf<UW_HotbarSlot> HotbarSlotClass, const TSubclassOf<UW_HotbarSelector> HotbarSelectorClass)
{
    this->UWHotbarSlots->ClearChildren();

    UCanvasPanelSlot* CanvasSlotContainer = CastChecked<UCanvasPanelSlot>(this->UWHotbarSlotsContainer->Slot);
    CanvasSlotContainer->SetSize({FVector2D(this->SlotCount * this->SlotSizeX, this->SlotSizeY)});
    CanvasSlotContainer->SetPosition({FVector2D(-(this->SlotCount * this->SlotSizeX) / 2, -(this->SlotSizeY + this->BottomMargin))});
    
    for (int i = 0; i < this->SlotCount; ++i)
    {
        UW_HotbarSlot* HotbarSlot = CreateWidget<UW_HotbarSlot>(this->GetWorld(), HotbarSlotClass);

        UContainerSlotData* Data = NewObject<UContainerSlotData>(this);
        Data->Index        = i;
        Data->Accumulated  = CastChecked<ACH_Master>(this->GetOwningPlayer()->GetCharacter())->GetInventorySlot(i);
        HotbarSlot->ContainerSlotData = Data;

        HotbarSlot->RenderSlot();

        UCanvasPanelSlot* CanvasSlot = this->UWHotbarSlots->AddChildToCanvas(HotbarSlot);
        CanvasSlot->SetSize({FVector2D(this->SlotSizeX, this->SlotSizeY)});
        CanvasSlot->SetPosition({static_cast<double>(i * this->SlotSizeX), 0.0f});

        continue;
    }

    UW_HotbarSelector* UWSelector = CreateWidget<UW_HotbarSelector>(this->GetWorld(), HotbarSelectorClass);
    this->Selector = this->UWHotbarSlots->AddChildToCanvas(UWSelector);
    this->Selector->SetSize({FVector2D(this->SlotSizeX + this->HotbarSelectorOverlap, this->SlotSizeY + this->HotbarSelectorOverlap)});
    this->OnSlotSelect();
    
    return;
}

void UW_Hotbar::OnSlotSelect() const
{
    this->Selector->SetPosition(
        {
        static_cast<double>(CastChecked<ACH_Master>(this->GetOwningPlayer()->GetCharacter())->GetSelectedQuickSlotIndex() * this->SlotSizeX) - this->HotbarSelectorOverlap / 2,
        0.0f - this->HotbarSelectorOverlap / 2
        }
    );
    
    return;
}

void UW_Hotbar::OnHotbarUpdate()
{
    for (int i = 0; i < this->SlotCount; ++i)
    {
        UW_HotbarSlot* HotbarSlot = CastChecked<UW_HotbarSlot>(this->UWHotbarSlots->GetChildAt(i));

        UContainerSlotData* Data        = NewObject<UContainerSlotData>(this);
        Data->Index                     = i;
        Data->Accumulated               = CastChecked<ACH_Master>(this->GetOwningPlayer()->GetCharacter())->GetInventorySlot(i);
        HotbarSlot->ContainerSlotData   = Data;

        HotbarSlot->RenderSlot();
        
        continue;
    }
    
    return;
}
