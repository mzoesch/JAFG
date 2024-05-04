// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/EscapeMenuAssets/ExitToMenuEscapeWidget.h"

#include "CommonHUD.h"
#include "System/JAFGGameInstance.h"

UExitToMenuEscapeWidget::UExitToMenuEscapeWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UExitToMenuEscapeWidget::OnThisTabPressed() const
{
    ACommonHUD* HUD = Cast<ACommonHUD>(this->GetOwningPlayer()->GetHUD());
    check( HUD )
    HUD->CreateWarningPopup(TEXT("Are you sure you want to exit to menu?"), [this] (const bool bAccepted) { this->OnPopUpClosed(bAccepted); });

    return;
}

void UExitToMenuEscapeWidget::OnTabPressed(const FString& Identifier)
{
    Super::OnTabPressed(Identifier);
}

void UExitToMenuEscapeWidget::OnPopUpClosed(const bool bAccepted) const
{
    if (bAccepted == false)
    {
        return;
    }

    /*
     * This is just temporary. We than should add the logic to destroy a session and exit to the menu later.
     */

    UJAFGGameInstance* Instance = Cast<UJAFGGameInstance>(this->GetGameInstance());
    check( Instance )

    Instance->RequestControlledShutdown();

    return;
}
