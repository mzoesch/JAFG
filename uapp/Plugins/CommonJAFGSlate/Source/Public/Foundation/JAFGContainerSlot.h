// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGDirtyUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Slot.h"
#include "Container.h"

#include "JAFGContainerSlot.generated.h"

class UJAFGContainerSlotTooltip;
class UImage;
class UJAFGTextBlock;
class UJAFGBorder;
class UJAFGContainer;

DECLARE_MULTICAST_DELEGATE(FContainerSlotChangedSignature)

/**
 * Designed to only handle rendering of the given data. But never to handle any interaction.
 * @see UJAFGReadOnlyContainerSlot
 */
UCLASS(NotBlueprintable)
class COMMONJAFGSLATE_API UJAFGContainerSlotDataBase : public UObject
{
    GENERATED_BODY()

public:

    FORCEINLINE virtual auto GetSlotValue(void) const -> FAccumulated PURE_VIRTUAL(UJAFGContainerSlotDataBase::GetSlotValue, return FAccumulated();)
    FORCEINLINE virtual auto GetSlotAmount(void) const -> accamount_t PURE_VIRTUAL(UJAFGContainerSlotDataBase::GetSlotAmount, return accamount_t();)
};

/** Placeholder */
UCLASS(NotBlueprintable)
class COMMONJAFGSLATE_API UJAFGReadOnlyContainerSlotData : public UJAFGContainerSlotDataBase
{
    GENERATED_BODY()

public:

    FAccumulated Content;

    FORCEINLINE virtual auto GetSlotValue(void) const -> FAccumulated override { return this->Content; }
    FORCEINLINE virtual auto GetSlotAmount(void) const -> accamount_t override { return this->Content.Amount; }
};

/** @note Only use for the OSD list entry passes and never to pass actual data around. */
UCLASS(NotBlueprintable)
class COMMONJAFGSLATE_API UJAFGContainerSlotData : public UJAFGContainerSlotDataBase
{
    GENERATED_BODY()

public:

    int32          Index;
    IContainer*    Owner;

    FORCEINLINE virtual auto GetSlot(void) const -> FSlot { return this->Owner->GetContainer().GetData()[this->Index]; }
    FORCEINLINE virtual auto GetSlotRef(void) const -> FSlot& { return this->Owner->GetContainer().GetData()[this->Index]; }
    FORCEINLINE virtual auto GetSlotValue(void) const -> FAccumulated override { return this->Owner->GetContainer().GetData()[this->Index].Content; }
    FORCEINLINE virtual auto GetSlotValueRef(void) const -> FAccumulated& { return this->Owner->GetContainer().GetData()[this->Index].Content; }
    FORCEINLINE virtual auto GetSlotAmount(void) const -> accamount_t override { return this->Owner->GetContainer().GetData()[this->Index].Content.Amount; }
    FORCEINLINE virtual auto GetSlotAmountRef(void) const -> accamount_t& { return this->Owner->GetContainer().GetData()[this->Index].Content.Amount; }
};

/**
 * A read-only slot widget that is not designed to be used with a container. But with pure data.
 * Handles rendering but does not handle any interaction.
 */
UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGReadOnlyContainerSlot : public UJAFGDirtyUserWidget, public IUserObjectListEntry
{
    GENERATED_BODY()

public:

    explicit UJAFGReadOnlyContainerSlot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    FORCEINLINE auto GetUncastedSlotData(void) const -> UJAFGContainerSlotDataBase* { return this->SlotData.Get(); }

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> void override;
    virtual auto NativeOnMouseLeave(const FPointerEvent& InMouseEvent) -> void override;
    virtual auto NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> FReply override;
    virtual auto NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> FReply override;
    // ~UUserWidget implementation

    // IUserObjectListEntry interface
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
    // ~IUserObjectListEntry interface

    // UJAFGDirtyUserWidget implementation
    virtual auto OnRefresh(void) -> void override;
    // ~UJAFGDirtyUserWidget implementation

    auto SetSlotData(UJAFGContainerSlotDataBase* InSlotData) -> void;
    auto RenderSlot(void) -> void;
    auto CreateToolTip(void) -> UJAFGContainerSlotTooltip*;

    const FColor HoverColor = FColor(0xFF, 0xFF, 0xFF, 0x15);
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGBorder> Border_Background;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGTextBlock> Text_Amount;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UImage> Image_Preview;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UJAFGBorder> Border_Foreground;

    UPROPERTY()
    TObjectPtr<UJAFGContainerSlotDataBase> SlotData = nullptr;
};

/** Optional widget to use as a foundation for containers inside JAFG. */
UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGContainerSlot : public UJAFGReadOnlyContainerSlot
{
    GENERATED_BODY()

public:

    explicit UJAFGContainerSlot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    FORCEINLINE auto GetSlotData(void) const -> UJAFGContainerSlotData&
    {
        return *Cast<UJAFGContainerSlotData>(this->GetUncastedSlotData());
    }

    template<class T>
    FORCEINLINE auto GetSlotData(void) const -> T&
    {
        return *Cast<T>(&this->GetSlotData());
    }

protected:

    // UUserWidget implementation
    virtual auto NativeConstruct(void) -> void override;
    virtual auto NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) -> FReply override;
    // ~UUserWidget implementation

    // IUserObjectListEntry interface
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
    // ~IUserObjectListEntry interface
};
