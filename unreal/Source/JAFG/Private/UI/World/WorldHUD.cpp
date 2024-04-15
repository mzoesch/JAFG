// Copyright 2024 mzoesch. All rights reserved.

#include "UI/World/WorldHUD.h"

#include "Misc/WorldSimulationSpectatorPawn.h"
#include "UI/Debug/DebugScreen.h"
#include "World/WorldCharacter.h"
#include "UI/Escape/EscapeMenu.h"
#include "UI/HUD/Crosshair.h"
#include "UI/HUD/Container/Hotbar.h"
#include "UI/HUD/Container/HotbarSelector.h"
#include "UI/HUD/Container/Slots/HotbarSlot.h"
#include "UI/MISC/ChatMenu.h"
#include "World/WorldPlayerController.h"

#define PLAYER_CONTROLLER \
    Cast<AWorldPlayerController>(this->GetOwningPlayerController())
#define SIMULATION_PLAYER_CONTROLLER \
    Cast<APlayerController>(this->GetOwningPlayerController())

AWorldHUD::AWorldHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AWorldHUD::BeginPlay(void)
{
    Super::BeginPlay();

#if WITH_EDITOR
    /*
     * May often happen if we simulate the game in the editor using PIE.
     * But should never happen in a packaged shipping game.
     */
    if (this->GetOwningPlayerController() == nullptr || (PLAYER_CONTROLLER == nullptr && SIMULATION_PLAYER_CONTROLLER == nullptr))
    {
        LOG_FATAL(LogCommonSlate, "Player controller is not valid. Discarding HUD setup.")
        return;
    }
#else /* WITH_EDITOR */
    /*
     * Simulation is not allowed in a packaged game. Therefore, we can directly discard the HUD setup
     * and log a fatal error.
     */
    if (this->GetOwningPlayerController() == nullptr || PLAYER_CONTROLLER == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Player controller is not valid.")
        return;
    }
#endif /* !WITH_EDITOR */

#if WITH_EDITOR
    /*
     * May often happen if we simulate the game in the editor using PIE.
     * But should never happen in a packaged shipping game.
     */
    if (this->GetOwningPawn() == nullptr || this->GetOwningPawn()->GetClass()->IsChildOf(AWorldCharacter::StaticClass()) == false)
    {
        if (this->GetOwningPlayerController() != nullptr && this->GetOwningPlayerController()->GetSpectatorPawn()->GetClass()->IsChildOf(AWorldSimulationSpectatorPawn::StaticClass()))
        {
            LOG_DISPLAY(LogCommonSlate, "Discarding HUD setup except for the debug screen because the owning pawn is a simulation spectator pawn.")

            check( this->WDebugScreenClass )
            this->WDebugScreen = CreateWidget<UDebugScreen>(this->GetWorld(), this->WDebugScreenClass);
            check( this->WDebugScreen )
            this->WDebugScreen->AddToViewport();
            this->WDebugScreen->SetVisibility(ESlateVisibility::Collapsed);

            return;
        }

        LOG_FATAL(LogCommonSlate, "Owning pawn is not a valid type for this HUD. Discarding HUD setup.")

        return;
    }
#else /* WITH_EDITOR */
    if (this->GetOwningPawn() == nullptr || this->GetOwningPawn()->GetClass()->IsChildOf(AWorldCharacter::StaticClass()) == false)
    {
        LOG_FATAL(LogCommonSlate, "Owning pawn is not a valid type for this HUD.")
        return;
    }
#endif /* !WITH_EDITOR */

    LOG_DISPLAY(LogCommonSlate, "Setting up World HUD in default configuration.")

    check( this->WDebugScreenClass)
    this->WDebugScreen = CreateWidget<UDebugScreen>(this->GetWorld(), this->WDebugScreenClass);
    check( this->WDebugScreen )
    this->WDebugScreen->AddToViewport();
    this->WDebugScreen->SetVisibility(ESlateVisibility::Collapsed);

    check( this->WHotbarClass )
    this->WHotbar = CreateWidget<UHotbar>(this->GetWorld(), this->WHotbarClass);
    check( this->WHotbar )
    this->WHotbar->AddToViewport();
    check( this->WHotbarSlotClass )
    check( this->WHotbarSelectorClass )
    this->WHotbar->InitializeHotbar(this->WHotbarSlotClass, this->WHotbarSelectorClass);

    check( this->WChatMenuClass )
    this->WChatMenu = CreateWidget<UChatMenu>(this->GetWorld(), this->WChatMenuClass);
    check( this->WChatMenu )
    this->WChatMenu->AddToViewport();

    check( this->WEscapeMenuClass )
    this->WEscapeMenu = CreateWidget<UEscapeMenu>(this->GetWorld(), this->WEscapeMenuClass);
    check( this->WEscapeMenu )
    this->WEscapeMenu->AddToViewport();

    check( this->WCrosshairClass )
    this->WCrosshair = CreateWidget<UCrosshair>(this->GetWorld(), this->WCrosshairClass);
    check( this->WCrosshair )
    this->WCrosshair->AddToViewport();

    return;
}

void AWorldHUD::RefreshHotbarSlots(void) const
{
    this->WHotbar->MarkAsDirty();
}

void AWorldHUD::RefreshHotbarSelectorLocation(void) const
{
    this->WHotbar->RefreshSelectorLocation();
}

void AWorldHUD::ToggleEscapeMenu(const bool bCollapsed) const
{
    check( PLAYER_CONTROLLER )
    PLAYER_CONTROLLER->ShowMouseCursor(bCollapsed == false, true);

    check( this->WCrosshair )
    if (bCollapsed)
    {
        this->WCrosshair->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        this->WCrosshair->SetVisibility(ESlateVisibility::Collapsed);
    }

    check( this->WEscapeMenu )
    this->WEscapeMenu->ToggleEscapeMenu(bCollapsed);

    return;
}

void AWorldHUD::ToggleChatMenu(const bool bCollapsed) const
{
    check( PLAYER_CONTROLLER )
    PLAYER_CONTROLLER->ShowMouseCursor(bCollapsed == false, true);

    check( this->WChatMenu )
    this->WChatMenu->ToggleChatMenu(bCollapsed);

    return;
}

void AWorldHUD::AddMessageToChat(const FString& Message) const
{
    check( this->WChatMenu )
    this->WChatMenu->AddMessageToChat(Message);
}

void AWorldHUD::ToggleDebugScreen(void) const
{
    check( this->WDebugScreen )
    this->WDebugScreen->Toggle();
}

#undef PLAYER_CONTROLLER
#undef SIMULATION_PLAYER_CONTROLLER
