// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "UI/HUD/Container/Slots/CommonSlot.h"

#include "CommonContainerSlot.generated.h"

JAFG_VOID

class UBorder;

UENUM()
namespace ESlotState
{

enum Type
{
    Normal,
    Hovered,
    Clicked,
};

}

UCLASS(Abstract, NotBlueprintable)
class JAFG_API UCommonContainerSlot : public UCommonSlot
{
    GENERATED_BODY()

protected:

    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:

    inline static constexpr FLinearColor NormalForeground  = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
    inline static constexpr FLinearColor HoveredForeground = FLinearColor(1.0f, 1.0f, 1.0f, 0.2f);
    inline static constexpr FLinearColor ClickedForeground = FLinearColor(1.0f, 1.0f, 1.0f, 0.4f);

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    TObjectPtr<UBorder> B_Foreground;

protected:

    bool bCurrentlyHovered = false;

    // UUserWidget implementation
    virtual auto NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> FReply override;
    virtual auto NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> FReply override;
    virtual auto NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> void override;
    virtual auto NativeOnMouseLeave(const FPointerEvent& InMouseEvent) -> void override;
    virtual auto NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> FReply override;
    virtual auto NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) -> void override;
    virtual auto NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) -> void override;
    virtual auto NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) -> void override;
    virtual auto NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) -> bool override;
    virtual auto NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) -> bool override;
    virtual auto NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) -> void override;
    // ~UUserWidget implementation

    virtual auto OnPrimaryClicked(void) -> void PURE_VIRTUAL(UCommonContainerSlot::OnPrimaryClicked)
    virtual auto OnSecondaryClicked(void) -> void PURE_VIRTUAL(UCommonContainerSlot::OnSecondaryClicked)

private:

    void ChangeForegroundState(const ESlotState::Type State) const;
};
