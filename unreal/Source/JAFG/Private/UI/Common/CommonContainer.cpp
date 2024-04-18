// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Common/CommonContainer.h"

#include "Components/TileView.h"
#include "UI/HUD/Container/Slots/SlateSlotData.h"
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

#undef OWNING_CHARACTER
#undef CHECK_OWNING_CHARACTER
#undef CHECKED_OWNING_CHARACTER
