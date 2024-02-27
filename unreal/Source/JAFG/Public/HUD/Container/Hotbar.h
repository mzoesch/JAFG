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

    bool bDirty;
    
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

protected:
    
    virtual void NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) override;

public:
        
    /**
     * Will set the bDirty flag to true and enables a refresh of the container on the next tick.
     *
     * TODO ...
     *      This is kinda a sketchy solution and should be redone.
     *      Why is there not bTickEnabled flag for Widgets, like in AActors?
     */
    FORCEINLINE void MarkAsDirty() { bDirty = true; }
    
    /** Can be called at any time to update the hotbar selector. The instance does not have to be marked as dirty. */
    void RefreshSlotSelector() const;
    
private:

    void RefreshHotbar();
    
public:
    
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UBorder* UWHotbarSlotsContainer;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCanvasPanel* UWHotbarSlots;

private:

    UPROPERTY()
    UCanvasPanelSlot* Selector;
};
