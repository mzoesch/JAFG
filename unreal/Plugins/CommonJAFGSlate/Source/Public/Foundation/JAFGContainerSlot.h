// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGDirtyUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Slot.h"

#include "JAFGContainerSlot.generated.h"

class UImage;
class UJAFGTextBlock;
class UJAFGBorder;
class UJAFGContainer;

/** @note Only use for the OSD list entry passes and never to pass actual data around. */
UCLASS(NotBlueprintable)
class UJAFGContainerSlotData : public UObject
{
    GENERATED_BODY()

public:

    int32          Index;
    /** @deprecated As we now provide the owner as well. */
    TArray<FSlot>* Container;
    IContainer*    Owner;

    FORCEINLINE auto GetSlot(void) const -> FSlot { return this->Container->GetData()[this->Index]; }
    FORCEINLINE auto GetSlotRef(void) const -> FSlot& { return this->Container->GetData()[this->Index]; }
    FORCEINLINE auto GetSlotValue(void) const -> FAccumulated { return this->Container->GetData()[this->Index].Content; }
    FORCEINLINE auto GetSlotValueRef(void) const -> FAccumulated& { return this->Container->GetData()[this->Index].Content; }
    FORCEINLINE auto GetSlotAmount(void) const -> accamount_t { return this->Container->GetData()[this->Index].Content.Amount; }
    FORCEINLINE auto GetSlotAmountRef(void) const -> accamount_t& { return this->Container->GetData()[this->Index].Content.Amount; }
};

/** Optional widget to use as a foundation for containers inside JAFG. */
UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGContainerSlot : public UJAFGDirtyUserWidget, public IUserObjectListEntry
{
    GENERATED_BODY()

public:

    explicit UJAFGContainerSlot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> FReply override;
    virtual auto NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> FReply override;
    virtual auto NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> void override;
    virtual auto NativeOnMouseLeave(const FPointerEvent& InMouseEvent) -> void override;
    // ~UUserWidget implementation

    // IUserObjectListEntry interface
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
    // ~IUserObjectListEntry interface

    const FColor HoverColor = FColor(0xFF, 0xFF, 0xFF, 0x15);
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGBorder> Border_Background;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGTextBlock> Text_Amount;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_Preview;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGBorder> Border_Foreground;

    // UJAFGDirtyUserWidget implementation
    virtual auto OnRefresh(void) -> void override;
    // ~UJAFGDirtyUserWidget implementation

    void RenderSlot(void) const;

private:

    UPROPERTY()
    TObjectPtr<UJAFGContainerSlotData> SlotData = nullptr;
};
