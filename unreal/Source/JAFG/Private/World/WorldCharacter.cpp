// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldCharacter.h"

#include "EnhancedPlayerInput.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/JAFGInputSubsystem.h"
#include "Player/World/WorldPlayerController.h"

#define PLAYER_CONTROLLER                                                            \
    Cast<AWorldPlayerController>(this->GetWorld()->GetFirstPlayerController())
#define ENHANCED_INPUT_SUBSYSTEM                                                     \
    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(                  \
        Cast<APlayerController>(                                                     \
            this->GetWorld()->GetFirstPlayerController()                             \
        )->GetLocalPlayer()                                                          \
    )

AWorldCharacter::AWorldCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWorldCharacter::BeginPlay(void)
{
    Super::BeginPlay();
}

void AWorldCharacter::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

#if !UE_BUILD_SHIPPING
    if (Cast<UEnhancedPlayerInput>(PLAYER_CONTROLLER->PlayerInput) == nullptr)
    {
        LOG_FATAL(LogWorldChar, "Player Controller is not using the Enhanced Input subsystem.")
        return;
    }
#endif /* !UE_BUILD_SHIPPING */

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_INPUT_SUBSYSTEM)
    {
        check( Subsystem )

        UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
        check( EnhancedInputComponent )

        UJAFGInputSubsystem* InputSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UJAFGInputSubsystem>();

        LOG_WARNING(LogWorldChar, "Enhanced Input Component is valid. Setting up input.")

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(InputSubsystem->MyInputMappingContext, 0);

        EnhancedInputComponent->BindAction(InputSubsystem->MyInputAction, ETriggerEvent::Started, this, &AWorldCharacter::OnTest);

    }

    return;
}

void AWorldCharacter::OnTest(const FInputActionValue& Value)
{
    LOG_WARNING(LogWorldChar, "OnTest called.")
}

#undef PLAYER_CONTROLLER
#undef ENHANCED_INPUT_SUBSYSTEM
