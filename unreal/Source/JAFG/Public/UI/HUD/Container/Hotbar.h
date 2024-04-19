// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "UI/Common/JAFGCommonDirtyWidget.h"

#include "Hotbar.generated.h"

JAFG_VOID

class UCanvasPanelSlot;
class UBorder;
class UCanvasPanel;
class UHotbarSelector;
class UHotbarSlot;

UCLASS(Abstract, Blueprintable)
class JAFG_API UHotbar : public UJAFGCommonDirtyWidget
{
    GENERATED_BODY()

public:

    void InitializeHotbar(const TSubclassOf<UHotbarSlot> HotbarSlotClass, const TSubclassOf<UHotbarSelector> HotbarSelectorClass);

    virtual void RefreshSelectorLocation(void) const;

private:

    /** To work properly, this must be the direct parent of UHotbar#CP_Slots. */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UCanvasPanel* CP_SlotContainerWrapper;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UBorder* B_SlotContainer;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
    UCanvasPanel* CP_Slots;

    /** Automatically generated. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UCanvasPanelSlot* CPS_Selector;

    /**
     * To also look not stretched on smaller resolutions, this should always be true:
     * UHotbar#HotbarSelectorOverlap % 2 == 0.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int HotbarSelectorOverlap = 16;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int BottomMargin = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int SlotSizeX = 64;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int SlotSizeY = 64;

protected:

    FDelegateHandle OnInventoryChanged_ClientDelegateHandle;
    FDelegateHandle OnQuickSlotLocationChanged_ClientDelegateHandle;

    virtual void NativeConstruct(void) override;
    virtual void OnRefresh(void) override;
};
