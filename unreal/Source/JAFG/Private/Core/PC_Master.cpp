// © 2023 mzoesch. All rights reserved.

#include "Core/PC_Master.h"

#include "Core/CH_Master.h"
#include "HUD/HUD_Master.h"

#define UIL_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define MASTER_CHARACTER                Cast<ACH_Master>(this->GetCharacter())
#define HUD                             Cast<AHUD_Master>(this->GetHUD())

void APC_Master::ShowMouseCursor(const bool bShow, const bool bCenter)
{
    if (bShow == true)
    {
        this->bShowMouseCursor          = true;
        this->bEnableClickEvents        = true;
        this->bEnableMouseOverEvents    = true;

        this->SetInputMode(FInputModeGameAndUI());

        if (bCenter)
        {
            if (const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(this->Player); !LocalPlayer->ViewportClient->Viewport || !LocalPlayer->ViewportClient->Viewport->IsForegroundWindow())
            {
                UIL_LOG(Warning, TEXT("ACH_Master::OnToggleInventory: Viewport is not foreground window!"))
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

void APC_Master::TransitToContainerState(const FString& Identifier, const bool bOpen, const bool bDestroy)
{
    this->ShowMouseCursor(bOpen, bOpen);
    MASTER_CHARACTER->TransitCharacterToContainerState(bOpen);

    if (bOpen == false)
    {
        /* All containers have the cursor preview that always must be destroyed. */
        HUD->SafeDestroyCursorPreview();
        
        if (bDestroy == true)
        {
            bool bSuccess = false;
            FString Err;
            HUD->RemoveContainer(Identifier, bSuccess, Err);

            if (bSuccess == false)
            {
                UIL_LOG(Error, TEXT("APC_Master::TransitToContainerState: Out Error: [%s]."), *Err)
                return;
            }

            return;
        }

        HUD->ToggleContainer(Identifier, false);
        return;
    }

    HUD->ToggleContainer(Identifier, true);
    return;
}

#undef UIL_LOG
#undef MASTER_CHARACTER
#undef HUD
