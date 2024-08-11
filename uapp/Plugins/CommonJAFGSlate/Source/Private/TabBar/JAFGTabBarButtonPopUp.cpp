// Copyright 2024 mzoesch. All rights reserved.

#include "TabBar/JAFGTabBarButtonPopUp.h"

#include "CommonHUD.h"


UJAFGTabBarButtonPopUp::UJAFGTabBarButtonPopUp(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGTabBarButtonPopUp::NativeOnThisTabClicked(void)
{
    ACommonHUD* HUD = Cast<ACommonHUD>(this->GetOwningPlayer()->GetHUD());
    check( HUD )

    if (this->Header.IsEmpty())
    {
        if (this->bIsYesNo)
        {
            HUD->CreateWarningPopupYesNo(this->Message, [this] (const bool bAccepted) -> void
            {
                this->OnPopUpClosed(bAccepted);
            });
        }
        else
        {
            HUD->CreateWarningPopup(this->Message);
        }
    }
    else
    {
        if (this->bIsYesNo)
        {
            HUD->CreateWarningPopupYesNo(this->Message, this->Header, [this] (const bool bAccepted) -> void
            {
                this->OnPopUpClosed(bAccepted);
            });
        }
        else
        {
            HUD->CreateWarningPopup(this->Message, this->Header);
        }
    }

    return;
}
