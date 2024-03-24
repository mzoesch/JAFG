// Copyright 2024 mzoesch. All rights reserved.

#include "Menu/MenuCharacter.h"

#include "Menu/MenuPlayerController.h"
#include "UI/Menu/JAFGFrontEnd.h"

AMenuCharacter::AMenuCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AMenuCharacter::BeginPlay()
{
    Super::BeginPlay();
    this->DisplayMenu();
    return;
}

void AMenuCharacter::DisplayMenu() const
{
    check( this->FrontEndClass )
    CreateWidget<UJAFGFrontEnd>(this->GetWorld(), this->FrontEndClass)->AddToViewport(0);

#if WITH_EDITOR
    if (Cast<AMenuPlayerController>(this->GetController()) == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("MenuCharacter::DisplayMenu: Controller is not a menu player controller."));
        return;
    }
#endif /* WITH_EDITOR */

    /* We can not center as the window is not yet created. */
    Cast<AMenuPlayerController>(this->GetController())->ShowMouseCursor(true, false);
    return;
}
