// © 2023 mzoesch. All rights reserved.

#include "HUD/Container/Container.h"

#include "Components/TileView.h"
#include "Core/CH_Master.h"
#include "HUD/HUD_Master.h"
#include "Lib/HUD/Container/ContainerSlotData.h"

#define CHARACTER Cast<ACH_Master>(GetOwningPlayerPawn())

void UW_Container::SetVisibility(ESlateVisibility InVisibility)
{
    Super::SetVisibility(InVisibility);

    if (InVisibility == ESlateVisibility::Hidden || InVisibility == ESlateVisibility::Collapsed)
    {
        this->OnDestroy();
    }

    return;
}

void UW_Container::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    this->MarkAsDirty();
    return;
}

void UW_Container::NativeTickImpl(const FGeometry& MyGeometry, const float InDeltaTime, bool& bOutDiscardTick)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (this->bDirty == false)
    {
        bOutDiscardTick = true;
        return;
    }
    
    this->bDirty = false;

    return;
}

void UW_Container::RefreshCharacterInventory()
{
    this->CharacterInventorySlots->ClearListItems();

    for (int i = 0; i < CHARACTER->GetInventorySize(); ++i)
    {
        UContainerSlotData* Data = NewObject<UContainerSlotData>(this);

        Data->Index        = i;
        Data->Accumulated  = CHARACTER->GetInventorySlot(i);

        this->CharacterInventorySlots->AddItem(Data);

        continue;
    }

    Cast<AHUD_Master>(this->GetOwningPlayer()->GetHUD())->RefreshCharacterHotbar();
    Cast<AHUD_Master>(this->GetOwningPlayer()->GetHUD())->RefreshCursorPreview();
    
    return;
}

#undef CHARACTER
