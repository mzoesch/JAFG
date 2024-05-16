// Copyright 2024 mzoesch. All rights reserved.

#include "JAFG/Public/WorldCore/WorldCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/CustomInputNames.h"
#include "Input/JAFGInputSubsystem.h"
#include "Player/WorldPlayerController.h"

class UEnhancedInputLocalPlayerSubsystem;

AWorldCharacter::AWorldCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->bReplicates = true;

    this->GetCapsuleComponent()->InitCapsuleSize(40.0f, 90.0f);

    this->FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    this->FirstPersonCameraComponent->SetupAttachment(this->GetCapsuleComponent());
    this->FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f));
    this->FirstPersonCameraComponent->bUsePawnControlRotation = true;
    this->FirstPersonCameraComponent->SetFieldOfView( 120.0f );

    this->GetCharacterMovement()->GravityScale               = 2.0f;
    this->GetCharacterMovement()->JumpZVelocity              = 700.0f;
    this->GetCharacterMovement()->AirControl                 = 2.0f;
    this->GetCharacterMovement()->MaxStepHeight              = 60.0f;
    this->GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

    return;
}

void AWorldCharacter::BeginPlay(void)
{
    Super::BeginPlay();

    if (this->IsLocallyControlled() == false)
    {
        return;
    }

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(this->GetController());

    if (WorldPlayerController == nullptr)
    {
        LOG_ERROR(LogWorldChar, "Owning World PlayerController is invalid. Cannot bind to events.")
        return;
    }

    this->EscapeMenuVisibilityChangedHandle = WorldPlayerController->SubscribeToEscapeMenuVisibilityChanged(ADD_SLATE_VIS_DELG(AWorldCharacter::OnEscapeMenuVisibilityChanged));

    return;
}

void AWorldCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (this->IsLocallyControlled() == false)
    {
        return;
    }

    AWorldPlayerController* WorldPlayerController = Cast<AWorldPlayerController>(this->GetController());

    if (WorldPlayerController == nullptr)
    {
        return;
    }

    if (WorldPlayerController->UnSubscribeToEscapeMenuVisibilityChanged(this->EscapeMenuVisibilityChangedHandle) == false)
    {
        LOG_ERROR(LogWorldChar, "Failed to unsubscribe from Escape Menu Visibility Changed event.")
    }

    return;
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
    if (ActionName == InputActions::Move)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnMove);
    }

    if (ActionName == InputActions::Look)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnLook);
    }

    if (ActionName == InputActions::Jump)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggerJump);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldCharacter::OnCompleteJump);
    }

    return;
}

void AWorldCharacter::OnMove(const FInputActionValue& Value)
{
    this->AddMovementInput(this->GetActorForwardVector(), Value.Get<FVector2D>().Y);
    this->AddMovementInput(this->GetActorRightVector(), Value.Get<FVector2D>().X);

    return;
}

void AWorldCharacter::OnLook(const FInputActionValue& Value)
{
    this->AddControllerYawInput(Value.Get<FVector2D>().X * 0.2f);
    this->AddControllerPitchInput(Value.Get<FVector2D>().Y * -0.2f);

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

    EnhancedInputComponent->BindAction(JAFGInputSubsystem->GetActionByName(ActionName), Event, this, Method);

    return;
}

void AWorldCharacter::OnEscapeMenuVisibilityChanged(const bool bVisible)
{
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
    check( Subsystem )

    UJAFGInputSubsystem* JAFGSubsystem = ULocalPlayer::GetSubsystem<UJAFGInputSubsystem>(this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
    check( JAFGSubsystem )

    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(JAFGSubsystem->GetContextByName(bVisible ? InputContexts::Escape : InputContexts::Foot), 0);

    return;
}

#pragma endregion Enhanced Input
