// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/EscapeMenuAssets/EscapeMenuExitToDesktopButton.h"

#include "System/JAFGGameInstance.h"

UEscapeMenuExitToDesktopButton::UEscapeMenuExitToDesktopButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->Message  = "Are you sure you want to exit to desktop?";
    this->bIsYesNo = true;

    return;
}

void UEscapeMenuExitToDesktopButton::OnPopUpClosed(const bool bAccepted)
{
    Super::OnPopUpClosed(bAccepted);

    if (bAccepted == false)
    {
        return;
    }

    UJAFGGameInstance* Instance = Cast<UJAFGGameInstance>(this->GetGameInstance());
    Instance->RequestControlledShutdown();

    return;
}
