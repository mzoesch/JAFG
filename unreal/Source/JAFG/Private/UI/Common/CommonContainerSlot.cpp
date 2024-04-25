// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Common/CommonContainerSlot.h"

#include "Components/Border.h"

void UCommonContainerSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    Super::NativeOnListItemObjectSet(ListItemObject);

    this->ChangeForegroundState(ESlotState::Normal);

    return;
}

FReply UCommonContainerSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    this->ChangeForegroundState(ESlotState::Clicked);

    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        this->OnPrimaryClicked();
    }
    else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
    {
        this->OnSecondaryClicked();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UCommonContainerSlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (this->bCurrentlyHovered)
    {
        this->ChangeForegroundState(ESlotState::Hovered);
    }
    else
    {
        this->ChangeForegroundState(ESlotState::Normal);
    }

    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UCommonContainerSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    this->ChangeForegroundState(ESlotState::Hovered);
    this->SetCursor(EMouseCursor::Hand);
    this->bCurrentlyHovered = true;

    return;
}

void UCommonContainerSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    this->ChangeForegroundState(ESlotState::Normal);
    this->SetCursor(EMouseCursor::Default);
    this->bCurrentlyHovered = false;

    return;
}

FReply UCommonContainerSlot::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    LOG_WARNING(LogCommonSlate, "Called.")
    return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void UCommonContainerSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
    LOG_WARNING(LogCommonSlate, "Called.")
}

void UCommonContainerSlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
    LOG_WARNING(LogCommonSlate, "Called.")
}

void UCommonContainerSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    Super::NativeOnDragLeave(InDragDropEvent, InOperation);
    LOG_WARNING(LogCommonSlate, "Called.")
}

bool UCommonContainerSlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    LOG_WARNING(LogCommonSlate, "Called.")
    return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool UCommonContainerSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    LOG_WARNING(LogCommonSlate, "Called.")
    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UCommonContainerSlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
    LOG_WARNING(LogCommonSlate, "Called.")
}

void UCommonContainerSlot::ChangeForegroundState(const ESlotState::Type State) const
{
    switch (State)
    {
    case ESlotState::Normal:
    {
        this->B_Foreground->SetBrushColor(UCommonContainerSlot::NormalForeground);
        break;
    }

    case ESlotState::Hovered:
    {
        this->B_Foreground->SetBrushColor(UCommonContainerSlot::HoveredForeground);
        break;
    }

    case ESlotState::Clicked:
    {
        this->B_Foreground->SetBrushColor(UCommonContainerSlot::ClickedForeground);
        break;
    }

    default:
    {
        LOG_ERROR(LogCommonSlate, "Unsupported slot state: %d", State)
        break;
    }
    }

    return;
}
