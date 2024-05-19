// Copyright 2024 mzoesch. All rights reserved.

#include "UI/FrontEnd/QuitGameFrontEndEntryWidget.h"

#include "CommonHUD.h"
#include "System/JAFGGameInstance.h"

UQuitGameFrontEndEntryWidget::UQuitGameFrontEndEntryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UQuitGameFrontEndEntryWidget::OnThisTabPressed(void) const
{
    ACommonHUD* HUD = Cast<ACommonHUD>(this->GetOwningPlayer()->GetHUD());
    check( HUD )
    HUD->CreateWarningPopup(TEXT("Are you sure you want to quit the game?"), [this] (const bool bAccepted) { this->OnPopUpClosed(bAccepted); });

    return;
}

void UQuitGameFrontEndEntryWidget::OnTabPressed(const FString& Identifier)
{
    Super::OnTabPressed(Identifier);
}

void UQuitGameFrontEndEntryWidget::OnPopUpClosed(const bool bAccepted) const
{
    if (bAccepted == false)
    {
        return;
    }

    UJAFGGameInstance* Instance = Cast<UJAFGGameInstance>(this->GetGameInstance());
    check( Instance )

    Instance->RequestControlledShutdown();

    return;
}
