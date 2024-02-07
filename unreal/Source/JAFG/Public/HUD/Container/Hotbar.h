// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/UW_Master.h"

#include "Hotbar.generated.h"

class UCanvasPanelSlot;
class UBorder;
class UCanvasPanel;
class UW_HotbarSlot;
class UW_HotbarSelector;

UCLASS()
class JAFG_API UW_Hotbar : public UW_Master
{
    GENERATED_BODY()

public:

    void InitializeHotbar(const TSubclassOf<UW_HotbarSlot> HotbarSlotClass, const TSubclassOf<UW_HotbarSelector> HotbarSelectorClass);

private:

    /* TODO Ofc move to Character */
    int SlotCount = 10;

public:

    /** To also look not stretched on smaller resolutions, this should always be true: HotbarSelectorOverlap % 2 == 0. */
    UPROPERTY(BlueprintReadOnly)
    int HotbarSelectorOverlap = 16;

    UPROPERTY(BlueprintReadOnly)
    int BottomMargin = 0;

    UPROPERTY(BlueprintReadOnly)
    int SlotSizeX = 64;

    UPROPERTY(BlueprintReadOnly)
    int SlotSizeY = 64;

public:

    void OnSlotSelect() const;
    void OnHotbarUpdate();

public:
    
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UBorder* UWHotbarSlotsContainer;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCanvasPanel* UWHotbarSlots;

private:

    UPROPERTY()
    UCanvasPanelSlot* Selector;
};
