// Copyright 2024 mzoesch. All rights reserved.

#include "UI/HUD/Container/Slots/CommonSlot.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/HUD/Container/Slots/SlateSlotData.h"

void UCommonSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    check( ListItemObject )
    this->SlateSlotData = Cast<USlateSlotData>(ListItemObject);
    check( this->SlateSlotData && L"Invalid List Item Object type." )

    this->RenderSlot();

    return;
}

void UCommonSlot::RenderSlot(void)
{
    check( this->I_Background )
    check( this->I_Preview )
    check( this->TB_Amount )

    this->I_Background->SetColorAndOpacity(FLinearColor(0.1f, 0.1f, 0.1f, 1.0f));

    this->TB_Amount->SetText(FText::FromString(L"65"));

    this->I_Preview->SetBrushFromTexture(nullptr);
    this->I_Preview->SetColorAndOpacity(FLinearColor::Transparent);

    return;
}
