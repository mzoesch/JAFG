// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGDirtyUserWidget.h"
#include "Hotbar.generated.h"

class UOverlaySlot;
class UOverlay;
class USizeBox;
class UCanvasPanelSlot;
class UHorizontalBox;

UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UHotbar : public UJAFGDirtyUserWidget
{
    GENERATED_BODY()

public:

    explicit UHotbar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation

    // UJAFGDirtyUserWidget implementation
    virtual void OnRefresh(void) override;
    // ~UJAFGDirtyUserWidget implementation

    void RenderHotbar(void);

public:

    inline static constexpr float SlotSize     { 50.0f };
    inline static constexpr float SelectorSize { 58.0f };

    void MoveSelectorToSlot(const int32 SlotIndex);

protected:

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = "true", AllowPrivateAccess = "true"))
    TObjectPtr<UOverlay> Overlay_Hotbar;

    UPROPERTY()
    TObjectPtr<USizeBox> CanvasSlot_Selector;

    UPROPERTY()
    TObjectPtr<UOverlaySlot> OverlaySlot_Selector;
};
