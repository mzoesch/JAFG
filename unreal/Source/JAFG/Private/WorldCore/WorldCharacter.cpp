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
#include "SettingsData/JAFGInputSubsystem.h"
#include "Player/WorldPlayerController.h"
#include "WorldCore/Chunk/CommonChunk.h"

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
        Subsystem->AddMappingContext(JAFGSubsystem->GetSafeContextValue(InputContexts::Foot), 0);

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

    else if (ActionName == InputActions::Crouch)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Triggered, &AWorldCharacter::OnTriggerCrouch);
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Completed, &AWorldCharacter::OnCompleteCrouch);
    }

    else if (ActionName == InputActions::Primary)
    {
        this->BindAction(ActionName, EnhancedInputComponent, ETriggerEvent::Started, &AWorldCharacter::OnStartedPrimary);
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

void AWorldCharacter::OnStartedPrimary(const FInputActionValue& Value)
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

    if (const voxel_t HitVoxel = TargetedChunk->GetLocalVoxelOnly(LocalHitVoxelKey); HitVoxel < ECommonVoxels::Num)
    {
        LOG_WARNING(LogWorldChar, "Cannot interact with common voxel type %d.", HitVoxel)
        return;
    }

    this->OnStartedPrimary_ServerRPC(Value);

    return;
}

void AWorldCharacter::OnStartedPrimary_ServerRPC_Implementation(const FInputActionValue& Value)
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

    // ReSharper disable once CppTooWideScopeInitStatement
    const voxel_t HitVoxel = TargetedChunk->GetLocalVoxelOnly(LocalHitVoxelKey);

    if (HitVoxel < ECommonVoxels::Num)
    {
        LOG_WARNING(LogWorldChar, "Increased strike for %s. Reason: Cannot interact with common voxel type %d.", *this->GetDisplayName(), HitVoxel)
        this->GetWorldPlayerController()->SafelyIncreaseStrikeCount();
        return;
    }

    TargetedChunk->ModifySingleVoxel(LocalHitVoxelKey, ECommonVoxels::Air);

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
    if (this->FirstPersonCameraComponent->FieldOfView == this->DefaultFieldOfView)
    {
        this->FirstPersonCameraComponent->SetFieldOfView(this->ZoomedFieldOfView);
        this->ThirdPersonCameraComponent->SetFieldOfView(this->ZoomedFieldOfView);
        this->ThirdPersonFrontCameraComponent->SetFieldOfView(this->ZoomedFieldOfView);
    }

    return;
}

void AWorldCharacter::OnCompleteZoomCameras(const FInputActionValue& Value)
{
    this->FirstPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView);
    this->ThirdPersonCameraComponent->SetFieldOfView(this->DefaultFieldOfView);
    this->ThirdPersonFrontCameraComponent->SetFieldOfView(this->DefaultFieldOfView);

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
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
    check( Subsystem )

    UJAFGInputSubsystem* JAFGSubsystem = ULocalPlayer::GetSubsystem<UJAFGInputSubsystem>(this->GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
    check( JAFGSubsystem )

    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(JAFGSubsystem->GetSafeContextValue(bVisible ? InputContexts::Escape : InputContexts::Foot), 0);

    return;
}

void AWorldCharacter::ToggleFly(void) const
{
    if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
    {
        this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        return;
    }

    this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

    return;
}

void AWorldCharacter::ToggleInputFly(void) const
{
    this->GetMyCharacterMovement()->bAllowInputFly = !this->GetMyCharacterMovement()->bAllowInputFly;
}

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

#pragma endregion Enhanced Input
