// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/UW_Master.h"

#include "UW_Hotbar.generated.h"

class UBorder;
class UCanvasPanel;
class UUW_HotbarSlot;

UCLASS()
class JAFG_API UUW_Hotbar : public UUW_Master
{
	GENERATED_BODY()

public:

	void InitializeHotbar(const TSubclassOf<UUW_HotbarSlot> UWHotbarSlotClass) const;

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

	void OnSlotSelect();

public:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* UWHotbarSlots;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* UWHotbarSlotsContainer;
	
};
