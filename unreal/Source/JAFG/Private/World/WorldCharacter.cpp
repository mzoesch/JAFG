// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "World/WorldPlayerController.h"

#define PLAYER_CONTROLLER        Cast<AWorldPlayerController>(this->GetWorld()->GetFirstPlayerController())
#define ENHANCED_INPUT_SUBSYSTEM ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Cast<APlayerController>(this->GetWorld()->GetFirstPlayerController())->GetLocalPlayer())

AWorldCharacter::AWorldCharacter(const FObjectInitializer& ObjectInitializer):
IMCFoot(nullptr), IAJump(nullptr), IALook(nullptr), IAMove(nullptr)
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWorldCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AWorldCharacter::OnTriggerJump(const FInputActionValue& Value)
{
    Super::Jump();
}

void AWorldCharacter::OnCompleteJump(const FInputActionValue& Value)
{
    Super::StopJumping();
}

void AWorldCharacter::OnLook(const FInputActionValue& Value)
{
    this->AddControllerYawInput(Value.Get<FVector2D>().X * 0.2f);
    this->AddControllerPitchInput(Value.Get<FVector2D>().Y * -0.2f);
}

void AWorldCharacter::OnMove(const FInputActionValue& Value)
{
    this->AddMovementInput(this->GetActorForwardVector(), Value.Get<FVector2D>().Y);
    this->AddMovementInput(this->GetActorRightVector(), Value.Get<FVector2D>().X);
}

void AWorldCharacter::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    check( PLAYER_CONTROLLER )

#if WITH_EDITOR
    if (Cast<UEnhancedPlayerInput>(PLAYER_CONTROLLER->PlayerInput) == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldCharacter::SetupPlayerInputComponent: Player Controller is not using the Enhanced Input subsystem."));
        return;
    }
#endif /* WITH_EDITOR */

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_INPUT_SUBSYSTEM)
    {
        check( Subsystem )

        check( this->IMCFoot )

        check( this->IAJump )
        check( this->IALook )
        check( this->IAMove )

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(this->IMCFoot, 0);
        UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

        EnhancedInputComponent->BindAction(this->IAJump, ETriggerEvent::Triggered, this, &AWorldCharacter::OnTriggerJump);
        EnhancedInputComponent->BindAction(this->IAJump, ETriggerEvent::Completed, this, &AWorldCharacter::OnCompleteJump);
        EnhancedInputComponent->BindAction(this->IALook, ETriggerEvent::Triggered, this, &AWorldCharacter::OnLook);
        EnhancedInputComponent->BindAction(this->IAMove, ETriggerEvent::Triggered, this, &AWorldCharacter::OnMove);
    }
    
    return;
}

#undef ENHANCED_INPUT_SUBSYSTEM
