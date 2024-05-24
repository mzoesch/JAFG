// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/EscapeMenuAssets/EscapeMenuResumeButton.h"

#include "Player/WorldPlayerController.h"
#include "TabBar/JAFGTabBar.h"

UEscapeMenuResumeButton::UEscapeMenuResumeButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UEscapeMenuResumeButton::NativeOnThisTabPressed(void)
{
    this->GetOwningTabBar<UJAFGTabBar>()->UnfocusAllTabs();

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(this->GetOwningPlayer());
    check( WorldPlayerController )

    WorldPlayerController->OnToggleEscapeMenu(FInputActionValue());

    return;
}
