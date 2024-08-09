// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/QuitGameFrontendButton.h"

#include "System/JAFGGameInstance.h"

UQuitGameFrontendButton::UQuitGameFrontendButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->Header   = TEXT("Confirm Exit");
    this->Message  = TEXT("Are you sure you want to quit?");
    this->bIsYesNo = true;

    return;
}

void UQuitGameFrontendButton::OnPopUpClosed(const bool bAccepted)
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
