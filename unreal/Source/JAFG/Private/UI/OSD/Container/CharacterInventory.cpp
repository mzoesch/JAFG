// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/Container/CharacterInventory.h"

#include "World/WorldCharacter.h"

#define OWNING_CHARACTER                                                    \
    Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter())
#define CHECK_OWNING_CHARACTER                                              \
    check( this->GetOwningPlayer() )                                        \
    check( this->GetOwningPlayer()->GetCharacter() )                        \
    check( Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter()) )
#define CHECKED_OWNING_CHARACTER                                            \
    check( this->GetOwningPlayer() )                                        \
    check( this->GetOwningPlayer()->GetCharacter() )                        \
    check( Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter()) ) \
    Cast<AWorldCharacter>(this->GetOwningPlayer()->GetCharacter())

void UCharacterInventory::NativeConstruct(void)
{
    Super::NativeConstruct();
}

void UCharacterInventory::OnRefresh(void)
{
    Super::OnRefresh();
}

void UCharacterInventory::Toggle(const bool bCollapsed)
{
    if (bCollapsed)
    {
        this->SetVisibility(ESlateVisibility::Collapsed);

        if (this->OnInventoryChanged_ClientDelegateHandle.IsValid())
        {
            if (OWNING_CHARACTER->UnsubscribeFromInventoryChanged(this->OnInventoryChanged_ClientDelegateHandle) == false)
            {
                LOG_ERROR(LogCommonSlate, "Failed to unsubscribe from inventory changed event.")
            }
        }

        return;
    }

    const FOnClientCharacterPropertyChangedEventSignature::FDelegate OnInventoryChanged_ClientDelegate =
        FOnClientCharacterPropertyChangedEventSignature::FDelegate::CreateUObject(this, &UCharacterInventory::MarkAsDirty);
    this->OnInventoryChanged_ClientDelegateHandle =
        OWNING_CHARACTER->SubscribeToInventoryChanged(OnInventoryChanged_ClientDelegate);

    this->SetVisibility(ESlateVisibility::Visible);

    this->MarkAsDirty();

    return;
}

#undef OWNING_CHARACTER
#undef CHECK_OWNING_CHARACTER
#undef CHECKED_OWNING_CHARACTER
