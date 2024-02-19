// © 2023 mzoesch. All rights reserved.

#include "HUD/OSD/Crosshair.h"

void UW_Crosshair::SetCrosshairState(const ECrosshairState NewState)
{
    if (CrosshairState == NewState)
    {
        return;
    }

    this->CrosshairState = NewState;
    this->OnCrosshairStateChanged();
    
    return;
}
