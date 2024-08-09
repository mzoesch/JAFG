// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/EscapeMenuAssets/EscapeMenuExitToMenuButton.h"

#include "LocalSessionSupervisorSubsystem.h"

UEscapeMenuExitToMenuButton::UEscapeMenuExitToMenuButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->Header   = TEXT("Confirm Exit");
    this->Message  = TEXT("Are you sure you want to exit to menu?");
    this->bIsYesNo = true;

    return;
}

void UEscapeMenuExitToMenuButton::OnPopUpClosed(const bool bAccepted)
{
    Super::OnPopUpClosed(bAccepted);

    if (bAccepted == false)
    {
        return;
    }

    ULocalSessionSupervisorSubsystem* LSSSS = this->GetGameInstance()->GetSubsystem<ULocalSessionSupervisorSubsystem>();
    LSSSS->LeaveSession();

    return;
}
