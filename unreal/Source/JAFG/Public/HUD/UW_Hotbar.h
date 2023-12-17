// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/UW_Master.h"

#include "UW_Hotbar.generated.h"

class UCanvasPanelSlot;
class UBorder;
class UCanvasPanel;
class UUW_HotbarSlot;
class UUW_HotbarSelector;

UCLASS()
class JAFG_API UUW_Hotbar : public UUW_Master
{
	GENERATED_BODY()

public:

	void InitializeHotbar(const TSubclassOf<UUW_HotbarSlot> UWHotbarSlotClass, const TSubclassOf<UUW_HotbarSelector> UWHotbarSelectorClass);

private:

	/* TODO Ofc move to Character */
	int SlotCount = 10;

    static constexpr int BottomMargin{25};
	
public:

	UPROPERTY(BlueprintReadOnly)
	int SlotSizeX = 100;

	UPROPERTY(BlueprintReadOnly)
	int SlotSizeY = 100;

public:

	void OnSlotSelect() const;

public:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UBorder* UWHotbarSlotsContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCanvasPanel* UWHotbarSlots;

private:

	UPROPERTY()
	class UCanvasPanelSlot* Selector;
	
};
