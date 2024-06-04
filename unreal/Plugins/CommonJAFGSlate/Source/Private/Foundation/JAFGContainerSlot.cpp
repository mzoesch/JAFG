// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/JAFGContainerSlot.h"

#include "CommonJAFGSlateDeveloperSettings.h"
#include "Container.h"
#include "Components/Image.h"
#include "Components/JAFGBorder.h"
#include "Components/JAFGTextBlock.h"
#include "Foundation/ContainerValueCursor.h"
#include "System/TextureSubsystem.h"

UJAFGContainerSlot::UJAFGContainerSlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGContainerSlot::NativeConstruct()
{
    Super::NativeConstruct();
    this->Border_Foreground->SetBrushColor(FColor::Transparent);
}

void UJAFGContainerSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    this->Border_Foreground->SetBrushColor(this->HoverColor);
    return;
}

void UJAFGContainerSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    this->Border_Foreground->SetBrushColor(FColor::Transparent);
    return;
}

FReply UJAFGContainerSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return FReply::Handled();
}

FReply UJAFGContainerSlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    IContainerOwner* ContainerOwner = Cast<IContainerOwner>(this->GetOwningPlayerPawn());
    if (ContainerOwner == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Container owner is invalid. Cannot proceed to handle mouse down.")
    }

    if (this->SlotData->GetSlotRef().OnPrimaryClicked(ContainerOwner))
    {
        this->MarkAsDirty();
        if (ContainerOwner->CursorValue != Accumulated::Null)
        {
            CreateWidget<UContainerValueCursor>(
                this->GetWorld(),
                GetDefault<UCommonJAFGSlateDeveloperSettings>()->ContainerValueCursorWidgetClass
            )->AddToViewport();
        }
    }

    return FReply::Handled();
}

void UJAFGContainerSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    this->SlotData = Cast<UJAFGContainerSlotData>(ListItemObject);

    this->RenderSlot();

    return;
}

void UJAFGContainerSlot::OnRefresh(void)
{
    this->RenderSlot();
}

void UJAFGContainerSlot::RenderSlot(void) const
{
    if (this->SlotData == nullptr)
    {
        LOG_WARNING(LogCommonSlate, "Slot data is invalid. Cannot proceed to render slot.")
        return;
    }

    if (this->SlotData->GetSlotValue() != Accumulated::Null)
    {
        this->Text_Amount->SetText(
            this->SlotData->GetSlotAmount() != 1
            ? FText::FromString(FString::FromInt(this->SlotData->GetSlotAmount()))
            : FText()
        );

        this->Image_Preview->SetBrushFromTexture(
            this->GetGameInstance()->GetSubsystem<UTextureSubsystem>()
                ->GetSafeTexture2D(this->SlotData->GetSlotValue().AccumulatedIndex
            )
        );
        this->Image_Preview->SetColorAndOpacity(FLinearColor::White);

        return;
    }

    this->Text_Amount->SetText(FText());
    this->Image_Preview->SetBrushFromTexture(nullptr);
    this->Image_Preview->SetColorAndOpacity(FLinearColor::Transparent);

    return;
}
