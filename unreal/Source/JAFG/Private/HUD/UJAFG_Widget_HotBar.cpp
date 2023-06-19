// © 2023 mzoesch. All rights reserved.


#include "HUD/UJAFG_Widget_HotBar.h"

#include "CORE/AGM_CORE.h"
#include "HUD/UJAFG_Widget_HotBarSlot.h"

void UJAFG_Widget_HotBar::UpdateSelectedSlot(int NewSlotID) {
	this->SelectedSlotID = NewSlotID;
	BIE_UpdateSelectedSlot();
	
	AGM_CORE* GM = Cast<AGM_CORE>(GetWorld()->GetAuthGameMode());
	check(GM)

	this->BP_JAFGWidgetHotBarSlot0->SetItemTexture(GM->TStone);
	this->BP_JAFGWidgetHotBarSlot1->SetItemTexture(GM->TBase);
	
	return;
}
