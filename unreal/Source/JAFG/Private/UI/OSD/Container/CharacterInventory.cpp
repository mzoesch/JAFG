// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/Container/CharacterInventory.h"

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
    }
    else
    {
        this->SetVisibility(ESlateVisibility::Visible);
        this->MarkAsDirty();
    }

    return;
}
