// Copyright 2024 mzoesch. All rights reserved.

#include "Player/JAFGPlayerController.h"

AJAFGPlayerController::AJAFGPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AJAFGPlayerController::ShowMouseCursor(const bool bShow, const bool bCenter)
{
    if (bShow == true)
    {
        this->bShowMouseCursor          = true;
        this->bEnableClickEvents        = true;
        this->bEnableMouseOverEvents    = true;

        /* TODO If we want to move how can we achieve this without the cursor disappearing? */
        // this->SetInputMode(FInputModeGameAndUI());
        this->SetInputMode(FInputModeUIOnly());

        if (bCenter)
        {
            if (const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(this->Player); !LocalPlayer->ViewportClient->Viewport || !LocalPlayer->ViewportClient->Viewport->IsForegroundWindow())
            {
                UE_LOG(LogTemp, Warning, TEXT("AJAFGPlayerController::ShowMouseCursor: Viewport is not foreground window. Discarding Center Mouse Request."))
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
