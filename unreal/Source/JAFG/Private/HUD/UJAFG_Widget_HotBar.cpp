// © 2023 mzoesch. All rights reserved.


#include "HUD/UJAFG_Widget_HotBar.h"

#include "CORE/AGM_CORE.h"
#include "HUD/UJAFG_Widget_HotBarSlot.h"

void UJAFG_Widget_HotBar::UpdateSelectedSlot(int NewSlotID) {
	this->SelectedSlotID = NewSlotID;
	BIE_UpdateSelectedSlot();
	
	// THIS WILL FAIL ON CLIENT
	// Save the textures elsewhere and then load them in the widget
	if (AGM_CORE* GM = Cast<AGM_CORE>(GetWorld()->GetAuthGameMode())) {
		this->BP_JAFGWidgetHotBarSlot0->SetItemTexture(GM->TStone);
		this->BP_JAFGWidgetHotBarSlot1->SetItemTexture(GM->TBase);
	}
	
	return;
}
