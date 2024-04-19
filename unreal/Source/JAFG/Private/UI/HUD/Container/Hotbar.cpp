// Copyright 2024 mzoesch. All rights reserved.

#include "UI/HUD/Container/Hotbar.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "UI/HUD/Container/HotbarSelector.h"
#include "UI/HUD/Container/Slots/HotbarSlot.h"
#include "UI/HUD/Container/Slots/SlateSlotData.h"
#include "World/WorldCharacter.h"

#define OWNING_CHARACTER                                                    \
    Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter())
#define CHECK_OWNING_CHARACTER                                              \
    check( this->GetOwningPlayer() )                                        \
    check( this->GetOwningPlayer()->GetCharacter() )                        \
    check( Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter()) )

void UHotbar::InitializeHotbar(const TSubclassOf<UHotbarSlot> HotbarSlotClass, const TSubclassOf<UHotbarSelector> HotbarSelectorClass)
{
    check( this->B_SlotContainer )
    check( this->CP_Slots )

    this->CP_Slots->ClearChildren();

    UCanvasPanelSlot* CanvasSlotContainer = Cast<UCanvasPanelSlot>(this->B_SlotContainer->Slot);
    check( CanvasSlotContainer )
    CanvasSlotContainer->SetSize({FVector2D(AWorldCharacter::HotbarSize * this->SlotSizeX, this->SlotSizeY)});
    CanvasSlotContainer->SetPosition(FVector2D(0.0, -this->BottomMargin));

    for (int i = 0; i < AWorldCharacter::HotbarSize; ++i)
    {
        UHotbarSlot* HotbarSlot = CreateWidget<UHotbarSlot>(this->GetWorld(), HotbarSlotClass);
        check( HotbarSlot )

        HotbarSlot->RenderSlot();

        UCanvasPanelSlot* CanvasPanelSlot = this->CP_Slots->AddChildToCanvas(HotbarSlot);
        check( CanvasPanelSlot )
        CanvasPanelSlot->SetSize(FVector2D(this->SlotSizeX, this->SlotSizeY));
        CanvasPanelSlot->SetPosition(FVector2D(i * this->SlotSizeX, 0.0));

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
    CHECK_OWNING_CHARACTER

    this->CPS_Selector->SetPosition(
        FVector2D(
            static_cast<double>(OWNING_CHARACTER->GetSelectedQuickSlotIndex() * this->SlotSizeX) - this->HotbarSelectorOverlap / 2.0,
            0.0 - this->HotbarSelectorOverlap / 2.0
        )
    );

    return;
}

void UHotbar::NativeConstruct(void)
{
    Super::NativeConstruct();

    CHECK_OWNING_CHARACTER

    const FOnClientCharacterPropertyChangedEventSignature::FDelegate OnInventoryChanged_ClientDelegate =
        FOnClientCharacterPropertyChangedEventSignature::FDelegate::CreateUObject(this, &UHotbar::MarkAsDirty);
    this->OnInventoryChanged_ClientDelegateHandle =
        OWNING_CHARACTER->SubscribeToInventoryChanged(OnInventoryChanged_ClientDelegate);

    const FOnClientCharacterPropertyChangedEventSignature::FDelegate OnQuickSlotLocationChanged_ClientDelegate =
        FOnClientCharacterPropertyChangedEventSignature::FDelegate::CreateUObject(this, &UHotbar::RefreshSelectorLocation);
    this->OnQuickSlotLocationChanged_ClientDelegateHandle =
        OWNING_CHARACTER->SubscribeToQuickSlotLocationChanged(OnQuickSlotLocationChanged_ClientDelegate);

    this->MarkAsDirty();

    return;
}

void UHotbar::OnRefresh(void)
{
    check( CP_Slots )

    this->RefreshSelectorLocation();

    for (int i = 0; i < AWorldCharacter::HotbarSize; ++i)
    {
        UHotbarSlot* HotbarSlot   = Cast<UHotbarSlot>(this->CP_Slots->GetChildAt(i));
        check( HotbarSlot )

        USlateSlotData* Data      = NewObject<USlateSlotData>(this);
        Data->Index               = i;
        Data->Accumulated         = OWNING_CHARACTER->GetInventorySlot(i);

        HotbarSlot->SlateSlotData = Data;

        HotbarSlot->RenderSlot();

        continue;
    }

    return;
}

#undef OWNING_CHARACTER
#undef CHECK_OWNING_CHARACTER
