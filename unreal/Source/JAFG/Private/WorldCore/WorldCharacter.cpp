// Copyright 2024 mzoesch. All rights reserved.

#include "JAFG/Public/WorldCore/WorldCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Input/CustomInputNames.h"
#include "Input/JAFGInputSubsystem.h"

class UEnhancedInputLocalPlayerSubsystem;

AWorldCharacter::AWorldCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;
}

void AWorldCharacter::BeginPlay(void)
{
    Super::BeginPlay();
}

#pragma region Enhanced Input

void AWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (this->IsLocallyControlled() == false)
    {
        LOG_ERROR(LogWorldChar, "Initialized on sv not loc ctrled.")
        return;
    }

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
        UJAFGInputSubsystem* JAFGSubsystem = ULocalPlayer::GetSubsystem<UJAFGInputSubsystem>(this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer());

        check( EnhancedInputComponent )
        check( Subsystem )
        check( JAFGSubsystem )

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(JAFGSubsystem->GetContextByName(InputContexts::Foot), 0);

        for (const FString& ActionName : JAFGSubsystem->GetAllActionNames())
        {
            this->BindAction(ActionName, EnhancedInputComponent);
        }

        return;
    }
}

void AWorldCharacter::BindAction(const FString& ActionName, UEnhancedInputComponent* EnhancedInputComponent)
{
    if (ActionName == InputActions::Jump)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggerJump);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldCharacter::OnCompleteJump);
    }

    return;
}

void AWorldCharacter::OnTriggerJump(const FInputActionValue& Value)
{
    Super::Jump();
}

void AWorldCharacter::OnCompleteJump(const FInputActionValue& Value)
{
    Super::StopJumping();
}

void AWorldCharacter::BindAction(
    const FString& ActionName,
    UEnhancedInputComponent* EnhancedInputComponent,
    const ETriggerEvent Event,
    void(AWorldCharacter::* Method) (const FInputActionValue& Value)
)
{
    /* Maybe we want to make this a member variable? */
    UJAFGInputSubsystem* JAFGInputSubsystem = this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->GetSubsystem<UJAFGInputSubsystem>();
    check( JAFGInputSubsystem )

    EnhancedInputComponent->BindAction(
        JAFGInputSubsystem->GetActionByName(ActionName),
        Event,
        this,
        Method
    );

    return;
}

#pragma endregion Enhanced Input
