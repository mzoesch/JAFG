// Copyright 2024 mzoesch. All rights reserved.

#include "Player/CommonPlayerController.h"

#include "Definitions.h"

ACommonPlayerController::ACommonPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void ACommonPlayerController::ShowMouseCursor(const bool bShow, const bool bCenter /* = true */ )
{
    if (this->IsLocalController() == false)
    {
        LOG_FATAL(LogCommonSlate, "Tried to change mouse cursor on a non-local controller.")
        return;
    }

    if (bShow == true)
    {
        this->bShowMouseCursor          = true;
        this->bEnableClickEvents        = true;
        this->bEnableMouseOverEvents    = true;

        FInputModeGameAndUI InputMode; InputMode.SetHideCursorDuringCapture(false);
        this->SetInputMode(InputMode);

        if (bCenter)
        {
            if (const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(this->Player); !LocalPlayer->ViewportClient->Viewport || !LocalPlayer->ViewportClient->Viewport->IsForegroundWindow())
            {
                LOG_WARNING(LogCommonSlate, "Viewport is not foreground window. Discarding Center Mouse Request.")
            }
            else
            {
                this->SetMouseLocation(LocalPlayer->ViewportClient->Viewport->GetSizeXY().X / 2, LocalPlayer->ViewportClient->Viewport->GetSizeXY().Y / 2);
            }
        }

        return;
    }

    this->bShowMouseCursor          = false;
    this->bEnableClickEvents        = false;
    this->bEnableMouseOverEvents    = false;

    this->SetInputMode(FInputModeGameOnly());

    return;
}

void ACommonPlayerController::ShowMouseCursor_ClientRPC_Implementation(const bool bShow, const bool bCenter)
{
    this->ShowMouseCursor(bShow, bCenter);
}
