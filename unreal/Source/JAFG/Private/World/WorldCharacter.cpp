// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Network/ChatComponent.h"
#include "UI/World/WorldHUD.h"
#include "World/WorldPlayerController.h"
#include "World/Chunk/CommonChunk.h"

#define PLAYER_CONTROLLER        Cast<AWorldPlayerController>(this->GetWorld()->GetFirstPlayerController())
#define HEAD_UP_DISPLAY          Cast<AWorldHUD>(this->GetWorld()->GetFirstPlayerController()->GetHUD())
#define ENHANCED_INPUT_SUBSYSTEM ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Cast<APlayerController>(this->GetWorld()->GetFirstPlayerController())->GetLocalPlayer())

AWorldCharacter::AWorldCharacter(const FObjectInitializer& ObjectInitializer):
IMCFoot(nullptr), IMCMenu(nullptr), IAJump(nullptr), IALook(nullptr), IAMove(nullptr), IAToggleEscapeMenu(nullptr)
{
    PrimaryActorTick.bCanEverTick = true;

    this->bReplicates = true;

    this->GetCapsuleComponent()->InitCapsuleSize(40.0f, 90.0f);

    this->ChatComponent = CreateDefaultSubobject<UChatComponent>(TEXT("ChatComponent"));

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

void AWorldCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AWorldCharacter::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
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

void AWorldCharacter::OnPrimary(const FInputActionValue& Value)
{
    const FTransform TraceStart = this->GetFirstPersonTraceStart();
    const FVector    TraceEnd   = TraceStart.GetLocation() + (TraceStart.GetRotation().Vector() * this->GetCharacterReach());

    const FCollisionQueryParams QueryParams = FCollisionQueryParams(FName(TEXT("PrimaryTrace")), false, this->GetOwner());

    FHitResult HitResult = FHitResult(ForceInit);
    this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart.GetLocation(), TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);

    if (HitResult.GetActor() == nullptr)
    {
        return;
    }

    if (HitResult.GetActor()->IsA(ACommonChunk::StaticClass()) == false)
    {
        return;
    }

    ACommonChunk* Chunk = Cast<ACommonChunk>(HitResult.GetActor());
    check( Chunk )

    const FVector    WorldHitLocation         = HitResult.Location - HitResult.Normal;
    const FIntVector LocalHitVoxelLocation = ACommonChunk::WorldToLocalVoxelLocation(WorldHitLocation);

    const int HitVoxel = Chunk->GetLocalVoxelOnly(LocalHitVoxelLocation);
    if (HitVoxel == ECommonVoxels::Null || HitVoxel == ECommonVoxels::Air)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldCharacter::OnPrimary: HitVoxel is not valid."))
        return;
    }

    this->OnPrimary_ServerRPC(Value);

    return;
}

void AWorldCharacter::OnPrimary_ServerRPC_Implementation(const FInputActionValue& Value)
{
    const FTransform TraceNoPitchStart = this->GetFirstPersonTraceStart();
    const FTransform TraceStart = FTransform(
        FQuat(FRotator(this->GetRemoteViewPitchAsDeg(), TraceNoPitchStart.Rotator().Yaw, TraceNoPitchStart.Rotator().Roll)),
        TraceNoPitchStart.GetLocation()
    );
    const FVector TraceEnd = TraceStart.GetLocation() + (TraceStart.GetRotation().Vector() * this->GetCharacterReach());

    const FCollisionQueryParams QueryParams = FCollisionQueryParams(FName(TEXT("PrimaryTrace")), false, this->GetOwner());

    FHitResult HitResult = FHitResult(ForceInit);
    this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart.GetLocation(), TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);

    if (HitResult.GetActor() == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldCharacter::OnPrimary_ServerRPC: HitResult.GetActor() is nullptr."))
        return;
    }

    if (HitResult.GetActor()->IsA(ACommonChunk::StaticClass()) == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldCharacter::OnPrimary_ServerRPC: HitResult.GetActor() is not a CommonChunk."))
        return;
    }

    ACommonChunk* Chunk = Cast<ACommonChunk>(HitResult.GetActor());
    check( Chunk )

    const FVector    WorldHitLocation         = HitResult.Location - HitResult.Normal;
    const FIntVector LocalHitVoxelLocation = ACommonChunk::WorldToLocalVoxelLocation(WorldHitLocation);

    const int HitVoxel = Chunk->GetLocalVoxelOnly(LocalHitVoxelLocation);
    if (HitVoxel == ECommonVoxels::Null || HitVoxel == ECommonVoxels::Air)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldCharacter::OnPrimary_ServerRPC: HitVoxel is not valid."))
        return;
    }

    Chunk->ModifySingleVoxel(LocalHitVoxelLocation, ECommonVoxels::Air);

    return;
}

void AWorldCharacter::OnSecondary(const FInputActionValue& Value)
{
}

/** Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void AWorldCharacter::OnToggleEscapeMenu(const FInputActionValue& Value)
{
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_INPUT_SUBSYSTEM)
    {
        if (Subsystem->HasMappingContext(this->IMCMenu))
        {
            HEAD_UP_DISPLAY->ToggleEscapeMenu(true);

            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(this->IMCFoot, 0);

            return;
        }

        HEAD_UP_DISPLAY->ToggleEscapeMenu(false);

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(this->IMCMenu, 0);

        return;
    }

    UE_LOG(LogTemp, Fatal, TEXT("AWorldCharacter::OnToggleEscapeMenu: Enhanced Input subsystem is not available."))

    return;
}

/** Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void AWorldCharacter::OnToggleChatMenu(const FInputActionValue& Value)
{
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_INPUT_SUBSYSTEM)
    {
        if (Subsystem->HasMappingContext(this->IMCChatMenu))
        {
            HEAD_UP_DISPLAY->ToggleChatMenu(true);

            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(this->IMCFoot, 0);

            return;
        }

        HEAD_UP_DISPLAY->ToggleChatMenu(false);

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(this->IMCChatMenu, 0);

        return;
    }

    UE_LOG(LogTemp, Fatal, TEXT("AWorldCharacter::OnToggleChatMenu: Enhanced Input subsystem is not available."))

    return;
}

void AWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    check( PLAYER_CONTROLLER )

#if WITH_EDITOR
    if (Cast<UEnhancedPlayerInput>(PLAYER_CONTROLLER->PlayerInput) == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldCharacter::SetupPlayerInputComponent: Player Controller is not using the Enhanced Input subsystem."))
        return;
    }
#endif /* WITH_EDITOR */

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_INPUT_SUBSYSTEM)
    {
        check( Subsystem )

        check( this->IMCFoot )
        check( this->IMCMenu )
        check( this->IMCChatMenu )

        check( this->IAJump )
        check( this->IALook )
        check( this->IAMove )

        check( this->IAPrimary )
        check( this->IASecondary )

        check( this->IAToggleEscapeMenu )
        check( this->IAToggleChatMenu )

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(this->IMCFoot, 0);
        UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

        EnhancedInputComponent->BindAction(this->IAJump, ETriggerEvent::Triggered, this, &AWorldCharacter::OnTriggerJump);
        EnhancedInputComponent->BindAction(this->IAJump, ETriggerEvent::Completed, this, &AWorldCharacter::OnCompleteJump);
        EnhancedInputComponent->BindAction(this->IALook, ETriggerEvent::Triggered, this, &AWorldCharacter::OnLook);
        EnhancedInputComponent->BindAction(this->IAMove, ETriggerEvent::Triggered, this, &AWorldCharacter::OnMove);

        EnhancedInputComponent->BindAction(this->IAPrimary, ETriggerEvent::Started, this, &AWorldCharacter::OnPrimary);
        EnhancedInputComponent->BindAction(this->IASecondary, ETriggerEvent::Started, this, &AWorldCharacter::OnSecondary);

        EnhancedInputComponent->BindAction(this->IAToggleEscapeMenu, ETriggerEvent::Started, this, &AWorldCharacter::OnToggleEscapeMenu);
        EnhancedInputComponent->BindAction(this->IAToggleChatMenu, ETriggerEvent::Started, this, &AWorldCharacter::OnToggleChatMenu);
    }

    return;
}

#undef ENHANCED_INPUT_SUBSYSTEM
