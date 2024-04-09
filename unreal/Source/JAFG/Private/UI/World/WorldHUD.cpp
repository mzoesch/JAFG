// Copyright 2024 mzoesch. All rights reserved.

#include "UI/World/WorldHUD.h"

#include "UI/Debug/DebugScreen.h"
#include "World/WorldCharacter.h"
#include "UI/Escape/EscapeMenu.h"
#include "UI/HUD/Crosshair.h"
#include "UI/MISC/ChatMenu.h"
#include "World/WorldPlayerController.h"

#define PLAYER_CONTROLLER Cast<AWorldPlayerController>(this->GetOwningPlayerController())

AWorldHUD::AWorldHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AWorldHUD::BeginPlay(void)
{
    Super::BeginPlay();

#if WITH_EDITOR
    /* May often happen if we simulate the game in the editor. But should never happen in a packaged shipping game. */
    if (PLAYER_CONTROLLER == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldHUD::BeginPlay: Player controller is not valid. Discarding HUD setup."))
        return;
    }
#else
    if (PLAYER_CONTROLLER == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldHUD::BeginPlay: Player controller is not valid."))
        return;
    }
#endif /* WITH_EDITOR */

#if WITH_EDITOR
    /* May often happen if we simulate the game in the editor. But should never happen in a packaged shipping game. */
    if (this->GetOwningPawn() == nullptr || this->GetOwningPawn()->GetClass()->IsChildOf(AWorldCharacter::StaticClass()) == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldHUD::BeginPlay: Owning pawn is not a valid type for this HUD. Discarding HUD setup."))
        return;
    }
#else
    if (this->GetOwningPawn() == nullptr || this->GetOwningPawn()->GetClass()->IsChildOf(AWorldCharacter::StaticClass()) == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldHUD::BeginPlay: Owning pawn is not a valid type for this HUD."))
        return;
    }
#endif /* WITH_EDITOR */

    check( this->WEscapeMenuClass )
    this->WEscapeMenu = CreateWidget<UEscapeMenu>(this->GetWorld(), this->WEscapeMenuClass);
    check( this->WEscapeMenu )
    this->WEscapeMenu->AddToViewport();

    check( this->WChatMenuClass )
    this->WChatMenu = CreateWidget<UChatMenu>(this->GetWorld(), this->WChatMenuClass);
    check( this->WChatMenu )
    this->WChatMenu->AddToViewport();

    check( this->WCrosshairClass )
    this->WCrosshair = CreateWidget<UCrosshair>(this->GetWorld(), this->WCrosshairClass);
    check( this->WCrosshair )
    this->WCrosshair->AddToViewport();

    check( this->WDebugScreenClass)
    this->WDebugScreen = CreateWidget<UDebugScreen>(this->GetWorld(), this->WDebugScreenClass);
    check( this->WDebugScreen )
    this->WDebugScreen->AddToViewport();
    this->WDebugScreen->SetVisibility(ESlateVisibility::Collapsed);

    return;
}

void AWorldHUD::ToggleEscapeMenu(const bool bCollapsed) const
{
    check( PLAYER_CONTROLLER )
    PLAYER_CONTROLLER->ShowMouseCursor(bCollapsed == false, true);

    this->WEscapeMenu->ToggleEscapeMenu(bCollapsed);
    check( this->WEscapeMenu )

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
