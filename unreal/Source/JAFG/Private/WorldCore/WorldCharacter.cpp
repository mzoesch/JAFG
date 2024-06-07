// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/WorldCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "JAFGSlateSettings.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Foundation/Hotbar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/CustomInputNames.h"
#include "Net/UnrealNetwork.h"
#include "SettingsData/JAFGInputSubsystem.h"
#include "Player/WorldPlayerController.h"
#include "UI/OSD/PlayerInventory.h"
#include "WorldCore/Cuboid.h"
#include "WorldCore/Character/CharacterReach.h"
#include "WorldCore/Chunk/CommonChunk.h"

#define ENHANCED_INPUT_SUBSYSTEM                                       \
    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(    \
        this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer() \
    )
#define JAFG_INPUT_SUBSYSTEM                                           \
    ULocalPlayer::GetSubsystem<UJAFGInputSubsystem>(                   \
        this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer() \
    )

AWorldCharacter::AWorldCharacter(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer.SetDefaultSubobjectClass<UMyCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    this->PrimaryActorTick.bCanEverTick = true;

    this->GetCapsuleComponent()->InitCapsuleSize(40.0f, 90.0f);

    this->NonFPMeshWrapper = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("NonFPMeshWrapper"));
    this->NonFPMeshWrapper->SetupAttachment(this->GetCapsuleComponent());
    this->NonFPMeshWrapper->SetOwnerNoSee(true);

    this->FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
    this->FirstPersonCameraComponent->SetupAttachment(this->GetCapsuleComponent());
    this->FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
    this->FirstPersonCameraComponent->bUsePawnControlRotation = true;
    this->FirstPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView);
    this->FirstPersonCameraComponent->OrthoWidth = this->DefaultOrthoWidth;

    this->ThirdPersonSpringArmComponent = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("ThirdPersonSpringArm"));
    this->ThirdPersonSpringArmComponent->SetupAttachment(this->GetCapsuleComponent());
    this->ThirdPersonSpringArmComponent->SocketOffset            = FVector(0.0f, 0.0f, 60.0f);
    this->ThirdPersonSpringArmComponent->bUsePawnControlRotation = true;
    this->ThirdPersonSpringArmComponent->TargetArmLength         = 256.0f;

    this->ThirdPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("ThirdPersonCamera"));
    this->ThirdPersonCameraComponent->SetupAttachment(this->ThirdPersonSpringArmComponent);
    this->ThirdPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView);
    this->ThirdPersonCameraComponent->OrthoWidth = this->DefaultOrthoWidth;
    this->ThirdPersonCameraComponent->Deactivate();

    this->ThirdPersonFrontSpringArmComponent = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("ThirdPersonFrontSpringArm"));
    this->ThirdPersonFrontSpringArmComponent->SetupAttachment(this->GetCapsuleComponent());
    this->ThirdPersonFrontSpringArmComponent->SocketOffset            = FVector(0.0f, 0.0f, 60.0f);
    this->ThirdPersonFrontSpringArmComponent->bUsePawnControlRotation = true;
    this->ThirdPersonFrontSpringArmComponent->TargetArmLength         = -256.0f;

    this->ThirdPersonFrontCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("ThirdPersonFrontCamera"));
    this->ThirdPersonFrontCameraComponent->SetupAttachment(this->ThirdPersonFrontSpringArmComponent);
    this->ThirdPersonFrontCameraComponent->SetFieldOfView(this->DefaultFieldOfView);
    this->ThirdPersonFrontCameraComponent->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
    this->ThirdPersonFrontCameraComponent->OrthoWidth = this->DefaultOrthoWidth;
    this->ThirdPersonFrontCameraComponent->Deactivate();

    this->RightHandComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RightHand"));
    this->RightHandComponent->SetupAttachment(this->RootComponent);

    return;
}

void AWorldCharacter::BeginPlay(void)
{
    Super::BeginPlay();

    this->AccumulatedPreview = this->GetWorld()->SpawnActor<ACuboid>(ACuboid::StaticClass(), FTransform(), FActorSpawnParameters());
    jcheck( this->AccumulatedPreview )
    this->ReattachAccumulatedPreview();

    if (this->IsLocallyControlled())
    {
        this->CharacterReach = this->GetWorld()->SpawnActor<ACharacterReach>(ACharacterReach::StaticClass(), FTransform(), FActorSpawnParameters());
        jcheck( this->CharacterReach )
        this->CharacterReach->AttachToComponent(this->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);

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
        this->GetMyCharacterMovement()->OnSprintStateChangedDelegate.AddLambda( [this] (const bool bSprinting)
        {
            this->UpdateFOVBasedOnSprintState();
        });

        WorldPlayerController->GetHUD<AWorldHUD>()->RegisterContainer(UPlayerInventory::Identifier, [] (void) -> TSubclassOf<UJAFGContainer>
        {
            return GetDefault<UJAFGSlateSettings>()->PlayerInventoryWidgetClass;
        });

        /* Let components set the current defaults for the active camera. */
        this->OnCameraChangedEvent.Broadcast();
    }

    if (UNetStatics::IsSafeServer(this))
    {
        LOG_VERY_VERBOSE(LogWorldChar, "Initializing container on server.")

        this->Container.Init(FSlot(Accumulated::Null), 10);
        this->AddToContainer(FAccumulated(ECommonVoxels::GetBaseVoxel()));
        this->AddToContainer(FAccumulated(ECommonVoxels::GetBaseVoxel() + 1, 10));
        this->AddToContainer(FAccumulated(ECommonVoxels::GetBaseVoxel() + 2, 99));

        LOG_VERY_VERBOSE(LogWorldChar, "Container initialized with %d slots.", this->Container.Num())

        this->PushContainerUpdatesToClient();
    }

    return;
}

void AWorldCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AWorldCharacter, RemoteSelectedAccumulatedPreview)

    return;
}

void AWorldCharacter::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    this->CurrentVelocity = this->GetVelocity();
    this->CurrentSpeed    = this->CurrentVelocity.Size();

    if (this->IsLocallyControlled() == false)
    {
        if (this->CurrentlyMiningLocalVoxel.IsSet())
        {
            this->CurrentDurationSameVoxelIsMined += DeltaSeconds;
        }

        return;
    }

    ACommonChunk*             TargetedChunk;
    FVector                   WorldHitLocation;
    FVector_NetQuantizeNormal WorldNormalHitLocation;
    FVoxelKey                 LocalHitVoxelKey;

    this->GetPOVTargetedData(
        TargetedChunk, WorldHitLocation, WorldNormalHitLocation, LocalHitVoxelKey,
        false, this->GetCharacterReach()
    );

    if (TargetedChunk == nullptr)
    {
        this->CharacterReach->Update(false);
    }
    else
    {
        this->CharacterReach->Update(WorldHitLocation);
    }

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

        this->ReattachAccumulatedPreview();

        return;
    });

    return;
}

void AWorldCharacter::UpdateFOVBasedOnSprintState(void) const
{
    if (this->GetMyCharacterMovement()->IsSprinting())
    {
        this->FirstPersonCameraComponent->SetFieldOfView(this->FirstPersonCameraComponent->FieldOfView * this->SprintFieldOfViewMultiplier);
    }
    else
    {
        this->FirstPersonCameraComponent->SetFieldOfView(this->bZooming ? this->ZoomedFieldOfView : this->DefaultFieldOfView);
    }

    return;
}

#pragma region Camera Stuff

void AWorldCharacter::ReattachAccumulatedPreview(void) const
{
    if (this->IsLocallyControlled() && this->FirstPersonCameraComponent->IsActive())
    {
        this->AccumulatedPreview->AttachToComponent(this->FirstPersonCameraComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }
    else
    {
        this->AccumulatedPreview->AttachToComponent(this->RightHandComponent, FAttachmentTransformRules::KeepRelativeTransform);
    }

    return;
}

void AWorldCharacter::UpdateAccumulatedPreview(const bool bReattach /* = false */) const
{
    if (bReattach)
    {
        this->ReattachAccumulatedPreview();
    }

    if (this->AccumulatedPreview)
    {
        this->AccumulatedPreview->SetActorRelativeTransform(this->GetAccumulatedPreviewRelativeTransformNoBob());

        if (this->IsLocallyControlled())
        {
            this->AccumulatedPreview->GenerateMesh(this->GetContainerValue(this->SelectedQuickSlotIndex).AccumulatedIndex);
        }
        else
        {
            this->AccumulatedPreview->GenerateMesh(this->RemoteSelectedAccumulatedPreview);
        }
    }

    return;
}

FTransform AWorldCharacter::GetAccumulatedPreviewRelativeTransformNoBob(void) const
{
    return this->FirstPersonCameraComponent->IsActive() ?
        FTransform(
            FRotator(0.0f, 14.0f, 0.0f),
            FVector(20.0f, 16.0f, -17.0f),
            FVector::One()
        ) :
        FTransform(
            FRotator::ZeroRotator,
            FVector::ZeroVector,
            FVector::One()
        );
}

#pragma endregion Camera Stuff

#pragma region Container

void AWorldCharacter::ChangeContainerSlot(const int32 Index, const accamount_t_signed Delta)
{
    if (UNetStatics::IsSafeClient(this))
    {
        LOG_FATAL(LogWorldChar, "Cannot change container slot on client.")
        return;
    }

    this->GetContainerValueRef(Index).SafeAddAmount(Delta);

    this->PushContainerUpdatesToClient();

    return;
}

void AWorldCharacter::PushContainerUpdatesToClient_ClientRPC_Implementation(const TArray<FSlot>& InContainer)
{
    LOG_VERY_VERBOSE(LogWorldChar, "Updating container on client. New slots: %d.", InContainer.Num())
    this->Container = InContainer;

    if (this->GetWorldHUD() && this->GetWorldHUD()->Hotbar)
    {
        this->GetWorldHUD()->Hotbar->MarkAsDirty();
    }

    this->UpdateAccumulatedPreview();

    return;
}

void AWorldCharacter::PushContainerUpdatesToClient(void)
{
    LOG_VERY_VERBOSE(LogWorldChar, "Pusing container updates to client with %d slots.", this->Container.Num())
    this->PushContainerUpdatesToClient_ClientRPC(this->Container);

    return;
}

#pragma endregion Container

#pragma region Enhanced Input

FDelegateHandle AWorldCharacter::SubscribeToContainerVisibleEvent(const FOnContainerVisibleSignature::FDelegate& Delegate)
{
    return this->OnContainerVisibleEvent.Add(Delegate);
}

bool AWorldCharacter::UnSubscribeToContainerVisibleEvent(const FDelegateHandle& Handle)
{
    return this->OnContainerVisibleEvent.Remove(Handle);
}

FDelegateHandle AWorldCharacter::SubscribeToContainerLostVisibilityEvent(const FOnContainerLostVisibilitySignature::FDelegate& Delegate)
{
    return this->OnContainerLostVisibilityEvent.Add(Delegate);
}

bool AWorldCharacter::UnSubscribeToContainerLostVisibilityEvent(const FDelegateHandle& Handle)
{
    return this->OnContainerLostVisibilityEvent.Remove(Handle);
}

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
        UEnhancedInputLocalPlayerSubsystem* Subsystem     = ENHANCED_INPUT_SUBSYSTEM;
        UJAFGInputSubsystem*                JAFGSubsystem = JAFG_INPUT_SUBSYSTEM;

        check( EnhancedInputComponent )
        check( Subsystem )
        check( JAFGSubsystem )

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(JAFGSubsystem->GetSafeContextValue(InputContexts::FootWalk), 0);

        for (const FString& ActionName : JAFGSubsystem->GetAllActionNames())
        {
            this->BindAction(ActionName, EnhancedInputComponent);
        }

        return;
    }
}

void AWorldCharacter::SetFootContextBasedOnCharacterState(const bool bClearOldMappings /* = true */, const int32 Priority /* = 0 */)
{
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_INPUT_SUBSYSTEM;

    if (bClearOldMappings)
    {
        Subsystem->ClearAllMappings();
    }

    Subsystem->AddMappingContext(this->GetSafeFootContextBasedOnCharacterState(), Priority);

    return;
}

UInputMappingContext* AWorldCharacter::GetFootContextBasedOnCharacterState(void) const
{
    UJAFGInputSubsystem* JAFGInputSubsystem =
        ULocalPlayer::GetSubsystem<UJAFGInputSubsystem>(
            this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer()
        );

    check( JAFGInputSubsystem )

    return this->GetFootContextBasedOnCharacterState(JAFGInputSubsystem);
}

UInputMappingContext* AWorldCharacter::GetSafeFootContextBasedOnCharacterState(void) const
{
    if (UInputMappingContext* Context = this->GetFootContextBasedOnCharacterState(); Context)
    {
        return Context;
    }

    LOG_FATAL(LogWorldChar, "Failed to get foot context based on character state.")

    return nullptr;
}

UInputMappingContext* AWorldCharacter::GetFootContextBasedOnCharacterState(UJAFGInputSubsystem* JAFGInputSubsystem) const
{
    if (this->GetMyCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
    {
        return JAFGInputSubsystem->GetSafeContextValue(InputContexts::FootFly);
    }

    return JAFGInputSubsystem->GetSafeContextValue(InputContexts::FootWalk);
}

UInputMappingContext* AWorldCharacter::GetSafeFootContextBasedOnCharacterState(UJAFGInputSubsystem* JAFGInputSubsystem) const
{
    if (UInputMappingContext* Context = this->GetFootContextBasedOnCharacterState(JAFGInputSubsystem); Context)
    {
        return Context;
    }

    LOG_FATAL(LogWorldChar, "Failed to get safe foot context based on character state.")

    return nullptr;
}

void AWorldCharacter::BindAction(const FString& ActionName, UEnhancedInputComponent* EnhancedInputComponent)
{
    if (ActionName == InputActions::Move)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggeredMove);
    }

    else if (ActionName == InputActions::Look)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggeredLook);
    }

    else if (ActionName == InputActions::Jump)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnStartedJump);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggerJump);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldCharacter::OnCompleteJump);
    }

    else if (ActionName == InputActions::Sprint)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnStartedSprint);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldCharacter::OnCompletedSprint);
    }

    else if (ActionName == InputActions::FlyUp)
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

    else if (ActionName == InputActions::FlyDown)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggerCrouch);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldCharacter::OnCompleteCrouch);
    }

    else if (ActionName == InputActions::Primary)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggeredPrimary);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldCharacter::OnCompletedPrimary);
    }

    else if (ActionName == InputActions::Secondary)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnStartedSecondary);
    }

    else if (ActionName == InputActions::ToggleContainer)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnStartedToggleContainer);
    }

    else if (ActionName == InputActions::ToggleCameras)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnToggleCameras);
    }

    else if (ActionName == InputActions::ZoomCameras)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggerZoomCameras);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldCharacter::OnCompleteZoomCameras);
    }

    else if (ActionName == InputActions::TogglePerspective)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnTogglePerspective);
    }

    else if (ActionName == InputActions::UpMaxFlySpeed)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggeredUpMaxFlySpeed);
    }

    else if (ActionName == InputActions::DownMaxFlySpeed)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggeredDownMaxFlySpeed);
    }

    else if (ActionName == InputActions::QuickSlotZero)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnQuickSlotZero);
    }

    else if (ActionName == InputActions::QuickSlotOne)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnQuickSlotOne);
    }

    else if (ActionName == InputActions::QuickSlotTwo)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnQuickSlotTwo);
    }

    else if (ActionName == InputActions::QuickSlotThree)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnQuickSlotThree);
    }

    else if (ActionName == InputActions::QuickSlotFour)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnQuickSlotFour);
    }

    else if (ActionName == InputActions::QuickSlotFive)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnQuickSlotFive);
    }

    else if (ActionName == InputActions::QuickSlotSix)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnQuickSlotSix);
    }

    else if (ActionName == InputActions::QuickSlotSeven)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnQuickSlotSeven);
    }

    else if (ActionName == InputActions::QuickSlotEight)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnQuickSlotEight);
    }

    else if (ActionName == InputActions::QuickSlotNine)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnQuickSlotNine);
    }

    else if (ActionName == InputActions::QuickSlotBitwise)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnQuickSlotBitwise);
    }

    return;
}

void AWorldCharacter::OnTriggeredMove(const FInputActionValue& Value)
{
    this->AddMovementInput(this->GetActorForwardVector(), Value.Get<FVector2D>().Y);
    this->AddMovementInput(this->GetActorRightVector(), Value.Get<FVector2D>().X);

    return;
}

void AWorldCharacter::OnTriggeredLook(const FInputActionValue& Value)
{
    this->AddControllerYawInput(Value.Get<FVector2D>().X * 0.2f);
    this->AddControllerPitchInput(Value.Get<FVector2D>().Y * -0.2f);

    return;
}

void AWorldCharacter::OnStartedJump(const FInputActionValue& Value)
{
    if (this->GetCharacterMovement()->IsFalling())
    {
        if (this->GetMyCharacterMovement()->bAllowInputFly && this->GetWorld()->GetTimeSeconds() - this->LastJumpStarted < this->JumpFlyModeDeactivationTime)
        {
            this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
            this->LastJumpStarted = this->GetWorld()->GetTimeSeconds();
            this->SetFootContextBasedOnCharacterState();
            return;
        }

        this->LastJumpStarted = this->GetWorld()->GetTimeSeconds();
        return;
    }

    if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
    {
        if (this->GetMyCharacterMovement()->bAllowInputFly && this->GetWorld()->GetTimeSeconds() - this->LastJumpStarted < this->JumpFlyModeDeactivationTime)
        {
            this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
            this->LastJumpStarted = this->GetWorld()->GetTimeSeconds();
            this->SetFootContextBasedOnCharacterState();
            return;
        }

        this->LastJumpStarted = this->GetWorld()->GetTimeSeconds();
        return;
    }

    this->LastJumpStarted = this->GetWorld()->GetTimeSeconds();

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

void AWorldCharacter::OnStartedSprint(const FInputActionValue& Value)
{
    this->GetMyCharacterMovement()->SetWantsToSprint(true);
}

void AWorldCharacter::OnCompletedSprint(const FInputActionValue& Value)
{
    this->GetMyCharacterMovement()->SetWantsToSprint(false);
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

void AWorldCharacter::OnTriggeredPrimary(const FInputActionValue& Value)
{
    ACommonChunk*             TargetedChunk;
    FVector                   WorldHitLocation;
    FVector_NetQuantizeNormal WorldNormalHitLocation;
    FVoxelKey                 LocalHitVoxelKey;

    this->GetPOVTargetedData(
        TargetedChunk, WorldHitLocation, WorldNormalHitLocation, LocalHitVoxelKey,
        false, this->GetCharacterReach()
    );

    if (TargetedChunk == nullptr)
    {
        if (this->CurrentlyMiningLocalVoxel.IsSet())
        {
            this->OnCompletedVoxelMinded_ServerRPC(false);
            this->CurrentlyMiningLocalVoxel.Reset();
            this->CurrentDurationSameVoxelIsMined = 0.0f;
        }

        return;
    }

    if (this->CurrentlyMiningLocalVoxel.IsSet() == false)
    {
        if (const voxel_t HitVoxel = TargetedChunk->GetLocalVoxelOnly(LocalHitVoxelKey); HitVoxel < ECommonVoxels::Num)
        {
            LOG_WARNING(LogWorldChar, "Cannot interact with common voxel type %d.", HitVoxel)
            return;
        }

        this->OnStartedVoxelMinded_ServerRPC(TargetedChunk->GetChunkKey(), LocalHitVoxelKey);
        this->CurrentlyMiningLocalVoxel       = LocalHitVoxelKey;
        this->CurrentDurationSameVoxelIsMined = 0.0f;

        return;
    }

    if (this->CurrentlyMiningLocalVoxel.GetValue() != LocalHitVoxelKey)
    {
        this->OnStartedVoxelMinded_ServerRPC(TargetedChunk->GetChunkKey(), LocalHitVoxelKey);
        this->CurrentlyMiningLocalVoxel       = LocalHitVoxelKey;
        this->CurrentDurationSameVoxelIsMined = 0.0f;

        return;
    }

    this->CurrentDurationSameVoxelIsMined += this->GetWorld()->GetDeltaSeconds();

    this->CharacterReach->Update(this->CurrentDurationSameVoxelIsMined / 0.5f);

    if (this->CurrentDurationSameVoxelIsMined >= 0.5f)
    {
        this->OnCompletedVoxelMinded_ServerRPC(true);
        this->CurrentlyMiningLocalVoxel.Reset();
        this->CurrentDurationSameVoxelIsMined = 0.0f;

        if (UNetStatics::IsSafeClient(this))
        {
            TargetedChunk->PredictSingleVoxelModification(LocalHitVoxelKey, ECommonVoxels::Air);
        }
    }

    return;
}

void AWorldCharacter::OnCompletedPrimary(const FInputActionValue& Value)
{
    this->OnCompletedVoxelMinded_ServerRPC(false);
    this->CurrentlyMiningLocalVoxel.Reset();
    this->CurrentDurationSameVoxelIsMined = 0.0f;
    this->CharacterReach->Update(0.0f);

    return;
}

void AWorldCharacter::OnStartedVoxelMinded_ServerRPC_Implementation(const FChunkKey& InTargetedChunk, const FVoxelKey& InLocalHitVoxelKey)
{
    ACommonChunk*             TargetedChunk;
    FVector                   WorldHitLocation;
    FVector_NetQuantizeNormal WorldNormalHitLocation;
    FVoxelKey                 LocalHitVoxelKey;

    this->GetPOVTargetedData(
        TargetedChunk, WorldHitLocation, WorldNormalHitLocation, LocalHitVoxelKey,
        this->IsLocallyControlled() == false, this->GetCharacterReach()
    );

    if (TargetedChunk == nullptr)
    {
        LOG_WARNING(LogWorldChar, "Increased strike for %s. Reason: Cannot interact with invalid chunk.", *this->GetDisplayName())
        this->GetWorldPlayerController()->SafelyIncreaseStrikeCount();

        this->CurrentlyMiningLocalVoxel.Reset();
        this->CurrentDurationSameVoxelIsMined = 0.0f;

        return;
    }

    if (LocalHitVoxelKey != InLocalHitVoxelKey)
    {
        LOG_WARNING(
            LogWorldChar,
            "Increased strike for %s. Reason: Remote and host hits differ: CL %s != SV %s.",
            *this->GetDisplayName(), *InLocalHitVoxelKey.ToString(), *LocalHitVoxelKey.ToString()
        )
        this->GetWorldPlayerController()->SafelyIncreaseStrikeCount();

        this->CurrentlyMiningLocalVoxel.Reset();
        this->CurrentDurationSameVoxelIsMined = 0.0f;

        return;
    }

    this->CurrentlyMiningLocalVoxel       = LocalHitVoxelKey;
    this->CurrentDurationSameVoxelIsMined = 0.0f;

    return;
}

void AWorldCharacter::OnCompletedVoxelMinded_ServerRPC_Implementation(const bool bClientBreak)
{
    if (bClientBreak == false)
    {
        this->CurrentlyMiningLocalVoxel.Reset();
        this->CurrentDurationSameVoxelIsMined = 0.0f;
        return;
    }

    ACommonChunk*             TargetedChunk;
    FVector                   WorldHitLocation;
    FVector_NetQuantizeNormal WorldNormalHitLocation;
    FVoxelKey                 LocalHitVoxelKey;

    this->GetPOVTargetedData(
        TargetedChunk, WorldHitLocation, WorldNormalHitLocation, LocalHitVoxelKey,
        this->IsLocallyControlled() == false, this->GetCharacterReach()
    );

    if (TargetedChunk == nullptr)
    {
        LOG_WARNING(LogWorldChar, "Increased strike for %s. Reason: Cannot interact with invalid chunk.", *this->GetDisplayName())
        this->GetWorldPlayerController()->SafelyIncreaseStrikeCount();

        this->CurrentlyMiningLocalVoxel.Reset();
        this->CurrentDurationSameVoxelIsMined = 0.0f;

        return;
    }

    if (this->CurrentDurationSameVoxelIsMined < 0.5f && FMath::IsNearlyEqual(this->CurrentDurationSameVoxelIsMined, .5f, .1f) == false)
    {
        LOG_WARNING(
            LogWorldChar,
            "Increased strike for %s. Reason: Voxel break was to short or to long.",
            *this->GetDisplayName()
        )
        this->GetWorldPlayerController()->SafelyIncreaseStrikeCount();

        this->CurrentlyMiningLocalVoxel.Reset();
        this->CurrentDurationSameVoxelIsMined = 0.0f;

        return;
    }

    TargetedChunk->ModifySingleVoxel(LocalHitVoxelKey, ECommonVoxels::Air);

    this->CurrentlyMiningLocalVoxel.Reset();
    this->CurrentDurationSameVoxelIsMined = 0.0f;

    return;
}

void AWorldCharacter::OnStartedSecondary(const FInputActionValue& Value)
{
    if (this->GetContainerValue(this->SelectedQuickSlotIndex) == Accumulated::Null)
    {
        return;
    }

    ACommonChunk*             TargetedChunk;
    FVector                   WorldHitLocation;
    FVector_NetQuantizeNormal WorldNormalHitLocation;
    FVoxelKey                 LocalHitVoxelKey;

    this->GetPOVTargetedData(
        TargetedChunk, WorldHitLocation, WorldNormalHitLocation, LocalHitVoxelKey,
        false, this->GetCharacterReach()
    );

    if (TargetedChunk == nullptr)
    {
        return;
    }

    FVector WorldTargetVoxelLocation = WorldHitLocation + WorldNormalHitLocation * WorldStatics::SingleVoxelSizeHalf;
            WorldTargetVoxelLocation = FVector(
                WorldTargetVoxelLocation.X - FMath::Fmod(WorldTargetVoxelLocation.X, WorldStatics::SingleVoxelSize),
                WorldTargetVoxelLocation.Y - FMath::Fmod(WorldTargetVoxelLocation.Y, WorldStatics::SingleVoxelSize),
                WorldTargetVoxelLocation.Z - FMath::Fmod(WorldTargetVoxelLocation.Z, WorldStatics::SingleVoxelSize)
            );
            /* This is kinda sketchy as it does not work around the x|y|z == 0 borders. */
            WorldTargetVoxelLocation = FVector(
                WorldTargetVoxelLocation.X + WorldStatics::SingleVoxelSizeHalf * FMath::Sign(WorldTargetVoxelLocation.X),
                WorldTargetVoxelLocation.Y + WorldStatics::SingleVoxelSizeHalf * FMath::Sign(WorldTargetVoxelLocation.Y),
                WorldTargetVoxelLocation.Z + WorldStatics::SingleVoxelSizeHalf * FMath::Sign(WorldTargetVoxelLocation.Z)
            );

    /* Only checking ourselves currently maybe we want to do some more checks before sending the RPC. */
    if (FVector::Dist(this->GetTorsoLocation(), WorldTargetVoxelLocation) < 100.0f)
    {
        return;
    }

    this->OnQuickSlot_ReliableServerRPC(this->SelectedQuickSlotIndex);
    this->OnStartedSecondary_ServerRPC(Value);

    return;
}

void AWorldCharacter::OnStartedSecondary_ServerRPC_Implementation(const FInputActionValue& Value)
{
    if (this->GetContainerValue(this->SelectedQuickSlotIndex) == Accumulated::Null)
    {
        LOG_WARNING(LogWorldChar, "Increased strike for %s. Reason: Cannot secondary interact with invalid voxel.", *this->GetDisplayName())
        this->GetWorldPlayerController()->SafelyIncreaseStrikeCount();
        return;
    }

    ACommonChunk*             TargetedChunk;
    FVector                   WorldHitLocation;
    FVector_NetQuantizeNormal WorldNormalHitLocation;
    FVoxelKey                 LocalHitVoxelKey;

    this->GetPOVTargetedData(
        TargetedChunk, WorldHitLocation, WorldNormalHitLocation, LocalHitVoxelKey,
        this->IsLocallyControlled() == false, this->GetCharacterReach()
    );

    if (TargetedChunk == nullptr)
    {
        LOG_WARNING(LogWorldChar, "Increased strike for %s. Reason: Cannot interact with invalid chunk.", *this->GetDisplayName())
        this->GetWorldPlayerController()->SafelyIncreaseStrikeCount();
        return;
    }

    const FVoxelKey LocalTargetVoxelKey = FIntVector(WorldNormalHitLocation + FVector(LocalHitVoxelKey));

    FVector WorldTargetVoxelLocation = WorldHitLocation + WorldNormalHitLocation * WorldStatics::SingleVoxelSizeHalf;
            WorldTargetVoxelLocation = FVector(
                WorldTargetVoxelLocation.X - FMath::Fmod(WorldTargetVoxelLocation.X, WorldStatics::SingleVoxelSize),
                WorldTargetVoxelLocation.Y - FMath::Fmod(WorldTargetVoxelLocation.Y, WorldStatics::SingleVoxelSize),
                WorldTargetVoxelLocation.Z - FMath::Fmod(WorldTargetVoxelLocation.Z, WorldStatics::SingleVoxelSize)
            );
            /* This is kinda sketchy as it does not work around the x|y|z == 0 borders. */
            WorldTargetVoxelLocation = FVector(
                WorldTargetVoxelLocation.X + WorldStatics::SingleVoxelSizeHalf * FMath::Sign(WorldTargetVoxelLocation.X),
                WorldTargetVoxelLocation.Y + WorldStatics::SingleVoxelSizeHalf * FMath::Sign(WorldTargetVoxelLocation.Y),
                WorldTargetVoxelLocation.Z + WorldStatics::SingleVoxelSizeHalf * FMath::Sign(WorldTargetVoxelLocation.Z)
            );

    /* Only checking ourselves currently maybe we want to do some more checks before sending the RPC. */
    if (FVector::Dist(this->GetTorsoLocation(), WorldTargetVoxelLocation) < 100.0f)
    {
        LOG_WARNING(LogWorldChar, "Increased strike for %s. Reason: Cannot interact with voxel too close to self.", *this->GetDisplayName())
        this->GetWorldPlayerController()->SafelyIncreaseStrikeCount();
        return;
    }

    TargetedChunk->ModifySingleVoxel(LocalTargetVoxelKey, this->GetContainerValue(this->SelectedQuickSlotIndex).AccumulatedIndex);
    this->ChangeContainerSlot(this->SelectedQuickSlotIndex, -1);

    return;
}

void AWorldCharacter::OnTriggeredUpMaxFlySpeed(const FInputActionValue& Value)
{
    if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
    {
        this->GetMyCharacterMovement()->IncrementFlySpeed();
    }

    return;
}

void AWorldCharacter::OnTriggeredDownMaxFlySpeed(const FInputActionValue& Value)
{
    if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
    {
        this->GetMyCharacterMovement()->DecrementFlySpeed();
    }

    return;
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

void AWorldCharacter::OnTriggerZoomCameras(const FInputActionValue& Value)
{
    if (this->bZooming == false)
    {
        this->FirstPersonCameraComponent->SetFieldOfView(this->ZoomedFieldOfView * (this->GetMyCharacterMovement()->IsSprinting() ? this->SprintFieldOfViewMultiplier : 1.0f));
        this->ThirdPersonCameraComponent->SetFieldOfView(this->ZoomedFieldOfView * (this->GetMyCharacterMovement()->IsSprinting() ? this->SprintFieldOfViewMultiplier : 1.0f));
        this->ThirdPersonFrontCameraComponent->SetFieldOfView(this->ZoomedFieldOfView * (this->GetMyCharacterMovement()->IsSprinting() ? this->SprintFieldOfViewMultiplier : 1.0f));

        this->bZooming = true;
    }

    return;
}

void AWorldCharacter::OnCompleteZoomCameras(const FInputActionValue& Value)
{
    this->FirstPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView * (this->GetMyCharacterMovement()->IsSprinting() ? this->SprintFieldOfViewMultiplier : 1.0f));
    this->ThirdPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView * (this->GetMyCharacterMovement()->IsSprinting() ? this->SprintFieldOfViewMultiplier : 1.0f));
    this->ThirdPersonFrontCameraComponent->SetFieldOfView(this->DefaultFieldOfView * (this->GetMyCharacterMovement()->IsSprinting() ? this->SprintFieldOfViewMultiplier : 1.0f));

    this->bZooming = false;

    return;
}

void AWorldCharacter::OnTogglePerspective(const FInputActionValue& Value)
{
    if (this->FirstPersonCameraComponent->ProjectionMode == ECameraProjectionMode::Perspective)
    {
        this->FirstPersonCameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);
        this->ThirdPersonCameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);
        this->ThirdPersonFrontCameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);
        return;
    }

    this->FirstPersonCameraComponent->SetProjectionMode(ECameraProjectionMode::Perspective);
    this->ThirdPersonCameraComponent->SetProjectionMode(ECameraProjectionMode::Perspective);
    this->ThirdPersonFrontCameraComponent->SetProjectionMode(ECameraProjectionMode::Perspective);

    return;
}

void AWorldCharacter::OnStartedToggleContainer(const FInputActionValue& Value)
{
    UEnhancedInputLocalPlayerSubsystem* Subsystem     = ENHANCED_INPUT_SUBSYSTEM; jcheck(     Subsystem )
    UJAFGInputSubsystem*                JAFGSubsystem = JAFG_INPUT_SUBSYSTEM;     jcheck( JAFGSubsystem )

    if (Subsystem->HasMappingContext(JAFGSubsystem->GetSafeContextValue(InputContexts::Container)))
    {
        Subsystem->ClearAllMappings();
        this->SetFootContextBasedOnCharacterState();

        this->OnContainerLostVisibilityEvent.Broadcast();

        return;
    }

    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(JAFGSubsystem->GetSafeContextValue(InputContexts::Container), 0);

    if (this->OnContainerVisibleEvent.IsBound() == false)
    {
        LOG_FATAL(LogWorldChar, "On Container Visible Event is not bound.")
        return;
    }

    this->OnContainerVisibleEvent.Broadcast(UPlayerInventory::Identifier);

    return;
}

#define QUICK_SLOT_0    0
#define QUICK_SLOT_1    1
#define QUICK_SLOT_2    2
#define QUICK_SLOT_3    3
#define QUICK_SLOT_4    4
#define QUICK_SLOT_5    5
#define QUICK_SLOT_6    6
#define QUICK_SLOT_7    7
#define QUICK_SLOT_8    8
#define QUICK_SLOT_9    9
#define QUICK_SLOT_MIN  0
#define QUICK_SLOT_MAX  9

void AWorldCharacter::OnQuickSlotZero(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_0);
    return;
}

void AWorldCharacter::OnQuickSlotOne(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_1);
    return;
}

void AWorldCharacter::OnQuickSlotTwo(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_2);
    return;
}

void AWorldCharacter::OnQuickSlotThree(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_3);
    return;
}

void AWorldCharacter::OnQuickSlotFour(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_4);
    return;
}

void AWorldCharacter::OnQuickSlotFive(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_5);
    return;
}

void AWorldCharacter::OnQuickSlotSix(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_6);
    return;
}

void AWorldCharacter::OnQuickSlotSeven(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_7);
    return;
}

void AWorldCharacter::OnQuickSlotEight(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_8);
    return;
}

void AWorldCharacter::OnQuickSlotNine(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_9);
    return;
}

void AWorldCharacter::OnQuickSlotBitwise(const FInputActionValue& Value)
{
    if (FMath::IsNearlyZero(Value.Get<FVector2D>().X))
    {
        return;
    }

    if (Value.Get<FVector2D>().X < 0.0f)
    {
        this->OnQuickSlot((this->SelectedQuickSlotIndex + 1) % (QUICK_SLOT_MAX + 1));
        return;
    }

    this->OnQuickSlot(this->SelectedQuickSlotIndex - 1 < QUICK_SLOT_MIN ? QUICK_SLOT_MAX : this->SelectedQuickSlotIndex - 1);

    return;
}

void AWorldCharacter::OnQuickSlot(const int8 Slot)
{
    this->SelectedQuickSlotIndex = Slot;
    this->GetWorldHUD()->Hotbar->MoveSelectorToSlot(Slot);

    this->OnQuickSlot_ServerRPC(Slot);

    this->UpdateAccumulatedPreview();

    return;
}

void AWorldCharacter::OnRep_RemoteSelectedAccumulatedPreview(void) const
{
    /* We ignore server requests as they might be "hanging" behind the client that is controlling this character. */
    if (this->IsLocallyControlled())
    {
        return;
    }

    this->UpdateAccumulatedPreview();

    return;
}

bool AWorldCharacter::OnQuickSlot_ServerRPC_Validate(const int8 Slot)
{
    return Slot >= QUICK_SLOT_MIN && Slot <= QUICK_SLOT_MAX;
}

void AWorldCharacter::OnQuickSlot_ServerRPC_Implementation(const int8 Slot)
{
    this->SelectedQuickSlotIndex           = Slot;
    this->RemoteSelectedAccumulatedPreview = this->GetContainerValue(Slot).AccumulatedIndex;

    /* We have to call manually, as we would not receive auto updates on the server when a client calls this RPC. */
    if (UNetStatics::IsSafeServer(this))
    {
        this->OnRep_RemoteSelectedAccumulatedPreview();
    }

    return;
}

bool AWorldCharacter::OnQuickSlot_ReliableServerRPC_Validate(const int8 Slot)
{
    return Slot >= QUICK_SLOT_MIN && Slot <= QUICK_SLOT_MAX;
}

void AWorldCharacter::OnQuickSlot_ReliableServerRPC_Implementation(const int8 Slot)
{
    this->SelectedQuickSlotIndex           = Slot;
    this->RemoteSelectedAccumulatedPreview = this->GetContainerValue(Slot).AccumulatedIndex;

    /* We have to call manually, as we would not receive auto updates on the server when a client calls this RPC. */
    if (UNetStatics::IsSafeServer(this))
    {
        this->OnRep_RemoteSelectedAccumulatedPreview();
    }

    return;
}

#undef QUICK_SLOT_0
#undef QUICK_SLOT_1
#undef QUICK_SLOT_2
#undef QUICK_SLOT_3
#undef QUICK_SLOT_4
#undef QUICK_SLOT_5
#undef QUICK_SLOT_6
#undef QUICK_SLOT_7
#undef QUICK_SLOT_8
#undef QUICK_SLOT_9
#undef QUICK_SLOT_MIN
#undef QUICK_SLOT_MAX

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

    EnhancedInputComponent->BindAction(JAFGInputSubsystem->GetSafeActionValue(ActionName), Event, this, Method);

    return;
}

void AWorldCharacter::OnEscapeMenuVisibilityChanged(const bool bVisible)
{
    UEnhancedInputLocalPlayerSubsystem* Subsystem     = ENHANCED_INPUT_SUBSYSTEM; jcheck(     Subsystem )
    UJAFGInputSubsystem*                JAFGSubsystem = JAFG_INPUT_SUBSYSTEM;     jcheck( JAFGSubsystem )

    Subsystem->ClearAllMappings();

    if (bVisible)
    {
        Subsystem->AddMappingContext(JAFGSubsystem->GetSafeContextValue(InputContexts::Escape), 0);
    }
    else
    {
        Subsystem->AddMappingContext(this->GetSafeFootContextBasedOnCharacterState(JAFGSubsystem), 0);
    }

    return;
}

#pragma endregion Enhanced Input

#pragma region Command Interface

void AWorldCharacter::ToggleFly(void)
{
    if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
    {
        this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        return;
    }

    this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

    this->SetFootContextBasedOnCharacterState();

    return;
}

void AWorldCharacter::ToggleInputFly(void) const
{
    this->GetMyCharacterMovement()->bAllowInputFly = !this->GetMyCharacterMovement()->bAllowInputFly;
}

#pragma endregion Command Interface

#pragma region World Interaction

void AWorldCharacter::GetPOVTargetedData(
    ACommonChunk*& OutChunk,
    FVector& OutWorldHitLocation,
    FVector_NetQuantizeNormal& OutWorldNormalHitLocation,
    FVoxelKey& OutLocalHitVoxelKey,
    const bool bUseRemotePitch,
    const float UnrealReach
) const
{
    OutChunk            = nullptr;
    OutWorldHitLocation = FVector::ZeroVector;

    const FTransform TraceStart = bUseRemotePitch ? this->GetNonLocalFirstPersonTraceStart() : this->GetFirstPersonTraceStart();
    const FVector    TraceEnd   = TraceStart.GetLocation() + (TraceStart.GetRotation().GetForwardVector() * UnrealReach);

    FCollisionQueryParams QueryParams = FCollisionQueryParams(FName(TEXT("POVTrace")), false, this);

    FHitResult HitResult = FHitResult(ForceInit);
    this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart.GetLocation(), TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);

    if (HitResult.GetActor() == nullptr)
    {
        return;
    }

    if (OutChunk = Cast<ACommonChunk>(HitResult.GetActor()); OutChunk != nullptr)
    {
        FVector FixedVector = HitResult.Location;
        if (FMath::IsNearlyZero(FixedVector.X, UE_DOUBLE_KINDA_SMALL_NUMBER)) { FixedVector.X = 0.0; }
        if (FMath::IsNearlyZero(FixedVector.Y, UE_DOUBLE_KINDA_SMALL_NUMBER)) { FixedVector.Y = 0.0; }
        if (FMath::IsNearlyZero(FixedVector.Z, UE_DOUBLE_KINDA_SMALL_NUMBER)) { FixedVector.Z = 0.0; }
        FixedVector -= HitResult.Normal;

        OutWorldHitLocation       = FixedVector;
        OutWorldNormalHitLocation = HitResult.Normal.GetSafeNormal();
        OutLocalHitVoxelKey       = ChunkStatics::WorldToLocalVoxelLocation(OutWorldHitLocation);
    }

    return;
}

#pragma endregion World Interaction

#undef ENHANCED_INPUT_SUBSYSTEM
#undef JAFG_INPUT_SUBSYSTEM
