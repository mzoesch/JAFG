// Copyright 2024 mzoesch. All rights reserved.

#include "UI/World/WorldHUD.h"

#include "World/WorldCharacter.h"
#include "UI/Escape/EscapeMenu.h"
#include "UI/MISC/ChatMenu.h"
#include "World/WorldPlayerController.h"

#define PLAYER_CONTROLLER Cast<AWorldPlayerController>(this->GetOwningPlayerController())

AWorldHUD::AWorldHUD(const FObjectInitializer& ObjectInitializer)
{
}

void AWorldHUD::BeginPlay()
{
    Super::BeginPlay();

#if WITH_EDITOR
    if (PLAYER_CONTROLLER == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldHUD::BeginPlay(): Player controller is not valid. Discarding HUD setup."))
        return;
    }
#else
    if (PLAYER_CONTROLLER == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldHUD::BeginPlay(): Player controller is not valid."))
        return;
    }
#endif /* WITH_EDITOR */
    
#if WITH_EDITOR
    /* May often happen if we simulate the game in the editor. But should never happen in a packaged shipping game. */
    if (this->GetOwningPawn() == nullptr || this->GetOwningPawn()->GetClass()->IsChildOf(AWorldCharacter::StaticClass()) == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldHUD::BeginPlay(): Owning pawn is not a valid type for this HUD. Discarding HUD setup."))
        return;
    }
#else
    if (this->GetOwningPawn() == nullptr || this->GetOwningPawn()->GetClass()->IsChildOf(AWorldCharacter::StaticClass()) == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldHUD::BeginPlay(): Owning pawn is not a valid type for this HUD."))
        return;
    }
#endif /* WITH_EDITOR */

    check( this->WEscapeMenuClass )
    this->WEscapeMenu = CreateWidget<UEscapeMenu>(this->GetWorld(), this->WEscapeMenuClass);
    this->WEscapeMenu->AddToViewport();

    check( this->WChatMenuClass )
    this->WChatMenu = CreateWidget<UChatMenu>(this->GetWorld(), this->WChatMenuClass);
    this->WChatMenu->AddToViewport();
    
    return;
}

void AWorldHUD::ToggleEscapeMenu(const bool bCollapsed) const
{
    PLAYER_CONTROLLER->ShowMouseCursor(bCollapsed == false, true);
    this->WEscapeMenu->ToggleEscapeMenu(bCollapsed);
}

void AWorldHUD::ToggleChatMenu(const bool bCollapsed) const
{
    PLAYER_CONTROLLER->ShowMouseCursor(bCollapsed == false, true);
    this->WChatMenu->ToggleChatMenu(bCollapsed);
}

void AWorldHUD::AddMessageToChat(const FString& Message)
{
    this->WChatMenu->AddMessageToChat(Message);
}

#undef PLAYER_CONTROLLER
