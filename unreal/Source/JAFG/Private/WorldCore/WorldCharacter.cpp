// Copyright 2024 mzoesch. All rights reserved.

#include "JAFG/Public/WorldCore/WorldCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/CustomInputNames.h"
#include "Input/JAFGInputSubsystem.h"
#include "Player/WorldPlayerController.h"
#include "WorldCore/Character/MyCharacterMovementComponent.h"

AWorldCharacter::AWorldCharacter(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer.SetDefaultSubobjectClass<UMyCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->GetCapsuleComponent()->InitCapsuleSize(40.0f, 90.0f);

    this->NonFPMeshWrapper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NonFPMeshWrapper"));
    this->NonFPMeshWrapper->SetupAttachment(this->GetCapsuleComponent());
    this->NonFPMeshWrapper->SetOwnerNoSee(true);

    this->FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    this->FirstPersonCameraComponent->SetupAttachment(this->GetCapsuleComponent());
    this->FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));//FVector(-10.0f, 0.0f, 60.0f));
    this->FirstPersonCameraComponent->bUsePawnControlRotation = true;
    this->FirstPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView);

    this->ThirdPersonSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonSpringArm"));
    this->ThirdPersonSpringArmComponent->SetupAttachment(this->GetCapsuleComponent());
    this->ThirdPersonSpringArmComponent->SocketOffset            = FVector(0.0f, 0.0f, 60.0f);
    this->ThirdPersonSpringArmComponent->bUsePawnControlRotation = true;
    this->ThirdPersonSpringArmComponent->TargetArmLength         = 256.0f;

    this->ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    this->ThirdPersonCameraComponent->SetupAttachment(this->ThirdPersonSpringArmComponent);
    this->ThirdPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView);
    this->ThirdPersonCameraComponent->Deactivate();

    this->ThirdPersonFrontSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonFrontSpringArm"));
    this->ThirdPersonFrontSpringArmComponent->SetupAttachment(this->GetCapsuleComponent());
    this->ThirdPersonFrontSpringArmComponent->SocketOffset            = FVector(0.0f, 0.0f, 60.0f);
    this->ThirdPersonFrontSpringArmComponent->bUsePawnControlRotation = true;
    this->ThirdPersonFrontSpringArmComponent->TargetArmLength         = -256.0f;

    this->ThirdPersonFrontCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonFrontCamera"));
    this->ThirdPersonFrontCameraComponent->SetupAttachment(this->ThirdPersonFrontSpringArmComponent);
    this->ThirdPersonFrontCameraComponent->SetFieldOfView(this->DefaultFieldOfView);
    this->ThirdPersonFrontCameraComponent->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
    this->ThirdPersonFrontCameraComponent->Deactivate();

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

    this->EscapeMenuVisibilityChangedHandle =
        WorldPlayerController->SubscribeToEscapeMenuVisibilityChanged(
            ADD_SLATE_VIS_DELG(AWorldCharacter::OnEscapeMenuVisibilityChanged)
        );

    this->ListenForCameraChangedEventWithNonFPMeshWrapper();

    /* Let components set the current defaults for the active camera. */
    this->OnCameraChangedEvent.Broadcast();

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

void AWorldCharacter::ListenForCameraChangedEventWithNonFPMeshWrapper(void)
{
    this->OnCameraChangedEvent.AddLambda( [this] (void)
    {
        this->NonFPMeshWrapper->SetOwnerNoSee(this->FirstPersonCameraComponent->IsActive());

        TArray<USceneComponent*> Children = TArray<USceneComponent*>();
        this->NonFPMeshWrapper->GetChildrenComponents(true, Children);
        for (USceneComponent* Child : Children)
        {
            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Child); Primitive)
            {
                Primitive->SetOwnerNoSee(this->FirstPersonCameraComponent->IsActive());
            }
        }

        return;
    });

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

    else if (ActionName == InputActions::Look)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnLook);
    }

    else if (ActionName == InputActions::Jump)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnStartedJump);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggerJump);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldCharacter::OnCompleteJump);
    }

    else if (ActionName == InputActions::Crouch)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggerCrouch);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldCharacter::OnCompleteCrouch);
    }

    else if (ActionName == InputActions::ToggleCameras)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnToggleCameras);
    }

    else if (ActionName == InputActions::ZoomFPCamera)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggerZoomFirstPersonCamera);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldCharacter::OnCompleteZoomFirstPersonCamera);
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

void AWorldCharacter::OnStartedJump(const FInputActionValue& Value)
{
    if (this->GetCharacterMovement()->IsFalling())
    {
        this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
        return;
    }

    if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
    {
        if (this->GetWorld()->GetTimeSeconds() - this->LastJumpTimeInFlyMode < this->JumpFlyModeDeactivationTime)
        {
            this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
            return;
        }

        this->LastJumpTimeInFlyMode = this->GetWorld()->GetTimeSeconds();
        return;
    }

    return;
}

void AWorldCharacter::OnTriggerJump(const FInputActionValue& Value)
{
    if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
    {
        Super::Jump();
        return;
    }

    if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
    {
        this->AddMovementInput(this->GetActorUpVector(), 1.0f);
        return;
    }

    return;
}

void AWorldCharacter::OnCompleteJump(const FInputActionValue& Value)
{
    Super::StopJumping();
}

void AWorldCharacter::OnTriggerCrouch(const FInputActionValue& Value)
{
    if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
    {
        Super::Crouch();
        return;
    }

    if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
    {
        this->AddMovementInput(this->GetActorUpVector(), -1.0f);
        return;
    }
}

void AWorldCharacter::OnCompleteCrouch(const FInputActionValue& Value)
{
    Super::UnCrouch();
}

void AWorldCharacter::OnToggleCameras(const FInputActionValue& Value)
{
    if (this->FirstPersonCameraComponent->IsActive())
    {
        this->FirstPersonCameraComponent->Deactivate();
        this->ThirdPersonCameraComponent->Activate();
    }

    else if (this->ThirdPersonCameraComponent->IsActive())
    {
        this->ThirdPersonCameraComponent->Deactivate();
        this->ThirdPersonFrontCameraComponent->Activate();
    }

    else if (this->ThirdPersonFrontCameraComponent->IsActive())
    {
        this->ThirdPersonFrontCameraComponent->Deactivate();
        this->FirstPersonCameraComponent->Activate();
        this->FirstPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView);
    }

    this->OnCameraChangedEvent.Broadcast();

    return;
}

void AWorldCharacter::OnTriggerZoomFirstPersonCamera(const FInputActionValue& Value)
{
    if (this->FirstPersonCameraComponent->IsActive() == false)
    {
        return;
    }

    if (this->FirstPersonCameraComponent->FieldOfView == this->DefaultFieldOfView)
    {
        this->FirstPersonCameraComponent->SetFieldOfView(this->ZoomedFieldOfView);
        return;
    }

    return;
}

void AWorldCharacter::OnCompleteZoomFirstPersonCamera(const FInputActionValue& Value)
{
    if (this->FirstPersonCameraComponent->IsActive() == false)
    {
        return;
    }

    this->FirstPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView);

    return;
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
