// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/EscapeMenuAssets/ExitToDesktopEscapeWidget.h"

#include "CommonHUD.h"
#include "System/JAFGGameInstance.h"

UExitToDesktopEscapeWidget::UExitToDesktopEscapeWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UExitToDesktopEscapeWidget::OnThisTabPressed(void) const
{
    ACommonHUD* HUD = Cast<ACommonHUD>(this->GetOwningPlayer()->GetHUD());
    check( HUD )
    HUD->CreateWarningPopup(TEXT("Are you sure you want to exit to desktop?"), [this] (const bool bAccepted) { this->OnPopUpClosed(bAccepted); });

    return;
}

void UExitToDesktopEscapeWidget::OnTabPressed(const FString& Identifier)
{
    Super::OnTabPressed(Identifier);
}

void UExitToDesktopEscapeWidget::OnPopUpClosed(const bool bAccepted) const
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
