// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Network/ChatComponent.h"
#include "Network/NetworkStatics.h"
#include "System/LocalPlayerChunkGeneratorSubsystem.h"
#include "UI/World/WorldHUD.h"
#include "World/WorldPlayerController.h"
#include "World/Chunk/CommonChunk.h"
#include "World/Chunk/LocalChunkValidator.h"

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
    this->LocalChunkValidator = CreateDefaultSubobject<ULocalChunkValidator>(TEXT("LocalChunkValidator"));

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

    if (UNetworkStatics::IsSafeClient(this) && this->IsLocallyControlled())
    {
        check( GEngine )
        check( this->GetWorld() )
        check( this->ChatComponent )

        const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(this->GetWorld());
        check( LocalPlayer )

        check( LocalPlayer->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>() )
        LocalPlayer->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>()->ConnectWithHyperlane();
    }

    return;
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
        FQuat(FRotator(
            /*
             * This is super sketchy. But currently we cannot determine if this pawn is the listen server's pawn
             * or not. In the future we should check with that.
             * Also see AWorldCharacter::GetRemoteViewPitchAsDeg.
             */
            TraceNoPitchStart.Rotator().Pitch == 0.0f
                ?
                    this->GetRemoteViewPitchAsDeg()
                :
                    TraceNoPitchStart.Rotator().Pitch,
            TraceNoPitchStart.Rotator().Yaw,
            TraceNoPitchStart.Rotator().Roll
        )),
        TraceNoPitchStart.GetLocation(),
        FVector::OneVector
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
    ACommonChunk*             TargetedChunk = nullptr;
    FVector                   TargetedWorldHitLocation = FVector::ZeroVector;
    FVector_NetQuantizeNormal TargetedWorldNormalHitLocation = FVector_NetQuantizeNormal::ZeroVector;
    FIntVector                TargetedLocalHitVoxelLocation = FIntVector::ZeroValue;
    this->GetTargetedVoxel(TargetedChunk, TargetedWorldHitLocation, TargetedWorldNormalHitLocation, TargetedLocalHitVoxelLocation, false, this->GetCharacterReach());

    if (TargetedChunk == nullptr)
    {
        return;
    }

    FVector             WorldTargetVoxelLocation    = TargetedWorldHitLocation + TargetedWorldNormalHitLocation * 50.0f;
    const FIntVector    LocalTargetVoxelLocation    = FIntVector(TargetedWorldNormalHitLocation + FVector(TargetedLocalHitVoxelLocation));
    WorldTargetVoxelLocation    = FVector(WorldTargetVoxelLocation.X - FMath::Fmod(WorldTargetVoxelLocation.X, 100.0f), WorldTargetVoxelLocation.Y - FMath::Fmod(WorldTargetVoxelLocation.Y, 100.0f), WorldTargetVoxelLocation.Z - FMath::Fmod(WorldTargetVoxelLocation.Z, 100.0f));
    /* This is kinda sketchy as it does not work around the x|y|z == 0 border. */
    WorldTargetVoxelLocation    = FVector(WorldTargetVoxelLocation.X + 50.0f * FMath::Sign(WorldTargetVoxelLocation.X), WorldTargetVoxelLocation.Y + 50.0f * FMath::Sign(WorldTargetVoxelLocation.Y), WorldTargetVoxelLocation.Z + 50.0f * FMath::Sign(WorldTargetVoxelLocation.Z));

    /* Only checking ourself currently maybe we want to do some more checks before sending the RPC. */
    if (FVector::Dist(this->GetTorsoLocation(), WorldTargetVoxelLocation) < 100.0f)
    {
        return;
    }

    this->OnSecondary_ServerRPC(Value);

    return;
}

void AWorldCharacter::OnSecondary_ServerRPC_Implementation(const FInputActionValue& Value)
{

    ACommonChunk*             TargetedChunk = nullptr;
    FVector                   TargetedWorldHitLocation = FVector::ZeroVector;
    FVector_NetQuantizeNormal TargetedWorldNormalHitLocation = FVector_NetQuantizeNormal::ZeroVector;
    FIntVector                TargetedLocalHitVoxelLocation = FIntVector::ZeroValue;
    this->GetTargetedVoxel(
        TargetedChunk,
        TargetedWorldHitLocation,
        TargetedWorldNormalHitLocation,
        TargetedLocalHitVoxelLocation,
        /*
         * This is super sketchy. But currently we cannot determine if this pawn is the listen server's pawn or not.
         * In the future we should check with that.
         */
        this->GetRemoteViewPitchAsDeg() != 0.0f,
        this->GetCharacterReach()
    );

    if (TargetedChunk == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldCharacter::OnSecondary_ServerRPC: TargetedChunk is nullptr."))
        return;
    }

    FVector             WorldTargetVoxelLocation    = TargetedWorldHitLocation + TargetedWorldNormalHitLocation * 50.0f;
    const FIntVector    LocalTargetVoxelLocation    = FIntVector(TargetedWorldNormalHitLocation + FVector(TargetedLocalHitVoxelLocation));
    WorldTargetVoxelLocation    = FVector(WorldTargetVoxelLocation.X - FMath::Fmod(WorldTargetVoxelLocation.X, 100.0f), WorldTargetVoxelLocation.Y - FMath::Fmod(WorldTargetVoxelLocation.Y, 100.0f), WorldTargetVoxelLocation.Z - FMath::Fmod(WorldTargetVoxelLocation.Z, 100.0f));
    /* This is kinda sketchy as it does not work around the x|y|z == 0 border. */
    WorldTargetVoxelLocation    = FVector(WorldTargetVoxelLocation.X + 50.0f * FMath::Sign(WorldTargetVoxelLocation.X), WorldTargetVoxelLocation.Y + 50.0f * FMath::Sign(WorldTargetVoxelLocation.Y), WorldTargetVoxelLocation.Z + 50.0f * FMath::Sign(WorldTargetVoxelLocation.Z));

    /* Only checking ourself currently maybe we want to do some more checks before sending the RPC. */
    if (FVector::Dist(this->GetTorsoLocation(), WorldTargetVoxelLocation) < 100.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldCharacter::OnSecondary_ServerRPC: Distance is less than 100."))
        return;
    }

    TargetedChunk->ModifySingleVoxel(LocalTargetVoxelLocation, /* Stone */ 2);

    return;
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

void AWorldCharacter::GetTargetedVoxel(ACommonChunk*& OutChunk, FVector& OutWorldHitLocation, FVector_NetQuantizeNormal& OutWorldNormalHitLocation, FIntVector& OutLocalHitVoxelLocation, const bool bUseRemotePitch, const float UnrealReach) const
{
    OutChunk = nullptr;
    OutLocalHitVoxelLocation = FIntVector::ZeroValue;

    const FTransform TraceStart = bUseRemotePitch ? this->GetFirstPersonTraceStart_DedServer() : this->GetFirstPersonTraceStart();
    const FVector TraceEnd = TraceStart.GetLocation() + (TraceStart.GetRotation().Vector() * UnrealReach);

    FCollisionQueryParams QueryParams = FCollisionQueryParams(FName(TEXT("CommonTrace")), false, this->GetOwner());

    FHitResult HitResult = FHitResult(ForceInit);
    this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart.GetLocation(), TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);

    if (HitResult.GetActor() == nullptr)
    {
        return;
    }

    if ((OutChunk = Cast<ACommonChunk>(HitResult.GetActor())) != nullptr)
    {
        OutWorldHitLocation = HitResult.Location - HitResult.Normal;
        OutWorldNormalHitLocation = HitResult.Normal;
        OutLocalHitVoxelLocation = ACommonChunk::WorldToLocalVoxelLocation(OutWorldHitLocation);
        return;
    }

    return;
}

#undef PLAYER_CONTROLLER
#undef HEAD_UP_DISPLAY
#undef ENHANCED_INPUT_SUBSYSTEM
