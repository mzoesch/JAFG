// Copyright 2024 mzoesch. All rights reserved.

#include "UI/HUD/Container/Slots/CommonSlot.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "System/TextureSubsystem.h"
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

    this->I_Background->SetColorAndOpacity(this->MyCommonWidgetBackgroundColor);

    if (this->SlateSlotData == nullptr || this->SlateSlotData->Accumulated.AccumulatedIndex == Accumulated::Null.AccumulatedIndex)
    {
        this->TB_Amount->SetText(FText::GetEmpty());
        this->I_Preview->SetBrushFromTexture(nullptr);
        this->I_Preview->SetColorAndOpacity(FLinearColor::Transparent);

        return;
    }

    check( this->GetGameInstance() )
    UTextureSubsystem* TextureSubsystem = this->GetGameInstance()->GetSubsystem<UTextureSubsystem>();
    check( TextureSubsystem )

    this->TB_Amount->SetText(
        this->SlateSlotData->Accumulated.Amount != 0
            ? FText::FromString(FString::FromInt(this->SlateSlotData->Accumulated.Amount))
            : FText::GetEmpty()
    );

    if (UTexture2D* Texture = TextureSubsystem->GetTexture2D(this->SlateSlotData->Accumulated))
    {
        this->I_Preview->SetBrushFromTexture(Texture);
        this->I_Preview->SetColorAndOpacity(FLinearColor::White);

        return;
    }

    this->I_Preview->SetBrushFromTexture(nullptr);
    this->I_Preview->SetColorAndOpacity(FLinearColor::Transparent);

    LOG_FATAL(LogTemp, "Failed to load any texture for an unkown hotbar slot.")

    return;
}
