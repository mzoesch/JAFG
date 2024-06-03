// Copyright 2024 mzoesch. All rights reserved.

#include "JAFG/Public/WorldCore/WorldCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/CustomInputNames.h"
#include "SettingsData/JAFGInputSubsystem.h"
#include "Player/WorldPlayerController.h"
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

    this->NonFPMeshWrapper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NonFPMeshWrapper"));
    this->NonFPMeshWrapper->SetupAttachment(this->GetCapsuleComponent());
    this->NonFPMeshWrapper->SetOwnerNoSee(true);

    this->FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    this->FirstPersonCameraComponent->SetupAttachment(this->GetCapsuleComponent());
    this->FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
    this->FirstPersonCameraComponent->bUsePawnControlRotation = true;
    this->FirstPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView);
    this->FirstPersonCameraComponent->OrthoWidth = this->DefaultOrthoWidth;

    this->ThirdPersonSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonSpringArm"));
    this->ThirdPersonSpringArmComponent->SetupAttachment(this->GetCapsuleComponent());
    this->ThirdPersonSpringArmComponent->SocketOffset            = FVector(0.0f, 0.0f, 60.0f);
    this->ThirdPersonSpringArmComponent->bUsePawnControlRotation = true;
    this->ThirdPersonSpringArmComponent->TargetArmLength         = 256.0f;

    this->ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    this->ThirdPersonCameraComponent->SetupAttachment(this->ThirdPersonSpringArmComponent);
    this->ThirdPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView);
    this->ThirdPersonCameraComponent->OrthoWidth = this->DefaultOrthoWidth;
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
    this->ThirdPersonFrontCameraComponent->OrthoWidth = this->DefaultOrthoWidth;
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

    /* Let components set the current defaults for the active camera. */
    this->OnCameraChangedEvent.Broadcast();

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

    this->OnStartedSecondary_ServerRPC(Value);

    return;
}

void AWorldCharacter::OnStartedSecondary_ServerRPC_Implementation(const FInputActionValue& Value)
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

    TargetedChunk->ModifySingleVoxel(LocalTargetVoxelKey, ECommonVoxels::Num);

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
    UEnhancedInputLocalPlayerSubsystem* Subsystem     = ENHANCED_INPUT_SUBSYSTEM;
    UJAFGInputSubsystem*                JAFGSubsystem = JAFG_INPUT_SUBSYSTEM;
    check(     Subsystem )
    check( JAFGSubsystem )

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
