// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Common/CommonContainer.h"

#include "Components/TileView.h"
#include "UI/HUD/Container/Slots/SlateSlotData.h"
#include "UI/OSD/Container/CursorHandPreview.h"
#include "UI/World/WorldHUD.h"
#include "World/WorldCharacter.h"

#define OWNING_CHARACTER                                                      \
    Cast<AWorldCharacter>(this->GetOwningPlayerPawn())
#define CHECK_OWNING_CHARACTER                                                \
    check( this->GetOwningPlayerPawn() )                                      \
    check( this->GetOwningPlayerPawn()->IsA(AWorldCharacter::StaticClass()) )
#define CHECKED_OWNING_CHARACTER                                              \
    check( this->GetOwningPlayerPawn() )                                      \
    check( this->GetOwningPlayerPawn()->IsA(AWorldCharacter::StaticClass()) ) \
    Cast<AWorldCharacter>(this->GetOwningPlayerPawn())

void UCommonContainer::NativeConstruct(void)
{
    Super::NativeConstruct();
}

void UCommonContainer::OnRefresh(void)
{
    this->RefreshCharacterInventorySlots();
    this->ResetCursorHand();
}

void UCommonContainer::RefreshCharacterInventorySlots(void)
{
    CHECK_OWNING_CHARACTER

    this->TV_CharacterInventorySlots->ClearListItems();

    for (int i = 0; i < OWNING_CHARACTER->GetInventorySize(); ++i)
    {
        USlateSlotData* Data = NewObject<USlateSlotData>(this);

        Data->Index        = i;
        Data->Accumulated  = OWNING_CHARACTER->GetInventorySlot(i);

        this->TV_CharacterInventorySlots->AddItem(Data);

        continue;
    }

    return;
}

void UCommonContainer::ResetCursorHand(void)
{
    if (this->W_CursorHand)
    {
        this->W_CursorHand->RemoveFromParent();
    }

    check( this->GetOwningPlayerPawn() )
    check( Cast<AWorldCharacter>(this->GetOwningPlayerPawn()) )

    if (Cast<AWorldCharacter>(this->GetOwningPlayerPawn())->GetCursorHand() == Accumulated::Null)
    {
        return;
    }

    check( this->GetOwningPlayer() )
    check( this->GetOwningPlayer()->GetHUD() )
    check( Cast<AWorldHUD>(this->GetOwningPlayer()->GetHUD()) )
    check( Cast<AWorldHUD>(this->GetOwningPlayer()->GetHUD())->WCursorHandPreviewClass )
    this->W_CursorHand = CreateWidget<UCursorHandPreview>(this, Cast<AWorldHUD>(this->GetOwningPlayer()->GetHUD())->WCursorHandPreviewClass);
    check( W_CursorHand )
    this->W_CursorHand->AddToViewport();

    return;
}

#undef OWNING_CHARACTER
#undef CHECK_OWNING_CHARACTER
#undef CHECKED_OWNING_CHARACTER
