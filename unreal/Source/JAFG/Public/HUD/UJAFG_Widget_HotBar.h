// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/UJAFG_Widget.h"

#include "UJAFG_Widget_HotBar.generated.h"


class UJAFG_Widget_HotBarSlot;


UCLASS()
class JAFG_API UJAFG_Widget_HotBar : public UJAFG_Widget {
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly)
	int SlotSizeX = 100;
	
	UPROPERTY(BlueprintReadOnly)
	int SlotSizeY = 100;

	UPROPERTY(BlueprintReadOnly)
	int SelectedSlotID = 0;
	void UpdateSelectedSlot(int NewSlotID);

	UFUNCTION(BlueprintImplementableEvent)
	void BIE_UpdateSelectedSlot();

public:
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UJAFG_Widget_HotBarSlot* BP_JAFGWidgetHotBarSlot0;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UJAFG_Widget_HotBarSlot* BP_JAFGWidgetHotBarSlot1;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UJAFG_Widget_HotBarSlot* BP_JAFGWidgetHotBarSlot2;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UJAFG_Widget_HotBarSlot* BP_JAFGWidgetHotBarSlot3;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UJAFG_Widget_HotBarSlot* BP_JAFGWidgetHotBarSlot4;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UJAFG_Widget_HotBarSlot* BP_JAFGWidgetHotBarSlot5;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UJAFG_Widget_HotBarSlot* BP_JAFGWidgetHotBarSlot6;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UJAFG_Widget_HotBarSlot* BP_JAFGWidgetHotBarSlot7;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UJAFG_Widget_HotBarSlot* BP_JAFGWidgetHotBarSlot8;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UJAFG_Widget_HotBarSlot* BP_JAFGWidgetHotBarSlot9;

};
