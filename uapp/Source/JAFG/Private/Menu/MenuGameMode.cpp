// Copyright 2024 mzoesch. All rights reserved.

#include "FrontEnd/FrontEndGameMode.h"

#include "Player/CommonPlayerController.h"
#include "UI/FrontEndHUD.h"

AFrontEndGameMode::AFrontEndGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->HUDClass              = AFrontEndHUD::StaticClass();
    this->PlayerControllerClass = ACommonPlayerController::StaticClass();

    this->bStartWithCursorVisible = true;
    this->bCanMoveCamera          = false;

    return;
}
