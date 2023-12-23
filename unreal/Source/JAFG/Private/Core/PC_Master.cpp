// © 2023 mzoesch. All rights reserved.


#include "Core/PC_Master.h"

void APC_Master::ShowMouseCursor(const bool bShow)
{
    if (bShow == true)
    {
        this->bShowMouseCursor = true;
        this->bEnableClickEvents = true;
        this->bEnableMouseOverEvents = true;

        return;
    }

    this->bShowMouseCursor = false;
    this->bEnableClickEvents = false;
    this->bEnableMouseOverEvents = false;

    return;
}
