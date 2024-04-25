// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Network/ChatComponent.h"
#include "Network/NetworkStatics.h"
#include "World/Chunk/LocalPlayerChunkGeneratorSubsystem.h"
#include "UI/World/WorldHUD.h"
#include "World/WorldPlayerController.h"
#include "World/Chunk/CommonChunk.h"
#include "World/Chunk/LocalChunkValidator.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "World/Entity/EntityWorldSubsystem.h"

#define PLAYER_CONTROLLER                                                            \
    Cast<AWorldPlayerController>(this->GetWorld()->GetFirstPlayerController())
#define HEAD_UP_DISPLAY                                                              \
    Cast<AWorldHUD>(this->GetWorld()->GetFirstPlayerController()->GetHUD())
#define CHECKED_HEAD_UP_DISPLAY                                                      \
    check( this->GetWorld() )                                                        \
    check( this->GetWorld()->GetFirstPlayerController() )                            \
    check( this->GetWorld()->GetFirstPlayerController()->GetHUD() )                  \
    check( Cast<AWorldHUD>(this->GetWorld()->GetFirstPlayerController()->GetHUD()) ) \
    HEAD_UP_DISPLAY
#define ENHANCED_INPUT_SUBSYSTEM                                                     \
    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(                  \
        Cast<APlayerController>(                                                     \
            this->GetWorld()->GetFirstPlayerController()                             \
        )->GetLocalPlayer()                                                          \
    )

AWorldCharacter::AWorldCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = true;
    this->SetActorTickInterval(1.0f);

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

    LOG_VERBOSE(LogWorldChar, "Called.")

    if (IS_PUSH_MODEL_ENABLED() == false)
    {
        LOG_FATAL(LogWorldChar, "Push Model is not enabled.")
        return;
    }

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

    if ((UNetworkStatics::IsSafeListenServer(this) && this->IsLocallyControlled()) || UNetworkStatics::IsSafeStandalone(this))
    {
        this->Inventory.SetNum(AWorldCharacter::InventoryStartSize, false);
        this->SelectedQuickSlotIndex = 0;

        this->AddToInventory(FAccumulated(ECommonVoxels::Air + 1, 5));
        this->AddToInventory(FAccumulated(ECommonVoxels::Air + 2, 50));
        this->AddToInventory(FAccumulated(ECommonVoxels::Air + 3, 500));

        /*
         * Unnecessary, but we want to make sure that the inventory is dirty.
         * All properties are dirty by default on Begin Play.
         */
        MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, Inventory, this)
        MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, SelectedQuickSlotIndex, this)
    }

    if (UNetworkStatics::IsSafeDedicatedServer(this))
    {
        this->Inventory.SetNum(AWorldCharacter::InventoryStartSize, false);
        this->SelectedQuickSlotIndex = 0;

        this->AddToInventory(FAccumulated(ECommonVoxels::Air + 1, 5));
        this->AddToInventory(FAccumulated(ECommonVoxels::Air + 2, 50));
        this->AddToInventory(FAccumulated(ECommonVoxels::Air + 3, 500));

        /*
         * Unnecessary, but we want to make sure that the inventory is dirty.
         * All properties are dirty by default on Begin Play.
         */
        MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, Inventory, this)
        MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, SelectedQuickSlotIndex, this)
    }

    return;
}

void AWorldCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (this->IsLocallyControlled())
    {
        this->OnInventoryChanged_ClientDelegate.Clear();
        this->OnQuickSlotLocationChanged_ClientDelegate.Clear();
    }

    return;
}

void AWorldCharacter::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWorldCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params = FDoRepLifetimeParams();

    Params.Condition    = ELifetimeCondition::COND_OwnerOnly;
    Params.bIsPushBased = true;
    DOREPLIFETIME_WITH_PARAMS_FAST(AWorldCharacter, Inventory, Params)
    DOREPLIFETIME_WITH_PARAMS_FAST(AWorldCharacter, SelectedQuickSlotIndex, Params)

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
    LOG_VERY_VERBOSE(LogWorldChar, "tirggered.")

    const FTransform TraceStart = this->GetFirstPersonTraceStart();
    const FVector    TraceEnd   = TraceStart.GetLocation() + (TraceStart.GetRotation().Vector() * this->GetCharacterReach());

    const FCollisionQueryParams QueryParams = FCollisionQueryParams(FName(TEXT("PrimaryTrace")), false, this->GetOwner());

    FHitResult HitResult = FHitResult(ForceInit);
    this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart.GetLocation(), TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);

    if (HitResult.GetActor() == nullptr)
    {
        LOG_VERY_VERBOSE(LogWorldChar, "No AActor Hit Result was found.")
        return;
    }

    if (HitResult.GetActor()->IsA(ACommonChunk::StaticClass()) == false)
    {
        LOG_VERBOSE(LogWorldChar, "AActor Hit Result is not a Common Chunk.")
        return;
    }

    ACommonChunk* Chunk = Cast<ACommonChunk>(HitResult.GetActor());
    check( Chunk )

    const FVector    WorldHitLocation      = HitResult.Location - HitResult.Normal;
    const FIntVector LocalHitVoxelLocation = ACommonChunk::WorldToLocalVoxelLocation(WorldHitLocation);

    // ReSharper disable once CppTooWideScopeInitStatement
    const int HitVoxel = Chunk->GetLocalVoxelOnly(LocalHitVoxelLocation);
    if (HitVoxel == ECommonVoxels::Null || HitVoxel == ECommonVoxels::Air)
    {
        LOG_WARNING(LogWorldChar, "Hit Voxel is not valid.")
        return;
    }

    this->OnPrimary_ServerRPC(Value);

    return;
}

void AWorldCharacter::OnPrimary_ServerRPC_Implementation(const FInputActionValue& Value)
{
    LOG_VERY_VERBOSE(LogWorldChar, "tirggered.")

    check( this->GetWorld() )

    const FTransform TraceNoPitchStart = this->GetFirstPersonTraceStart();
    const FTransform TraceStart = FTransform(
        FQuat(FRotator(
            /*
             * This is super sketchy. But currently we cannot determine if this pawn is the listen server's pawn
             * or not. In the future, we should check with that.
             * Also see AWorldCharacter#GetRemoteViewPitchAsDeg for more information.
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
        LOG_WARNING(LogWorldChar, "No AActor Hit Result was found.")
        return;
    }

    if (HitResult.GetActor()->IsA(ACommonChunk::StaticClass()) == false)
    {
        LOG_WARNING(LogWorldChar, "AActor Hit Result is not a Common Chunk.")
        return;
    }

    ACommonChunk* Chunk = Cast<ACommonChunk>(HitResult.GetActor());
    check( Chunk )

    const FVector    WorldHitLocation      = HitResult.Location - HitResult.Normal;
    const FIntVector LocalHitVoxelLocation = ACommonChunk::WorldToLocalVoxelLocation(WorldHitLocation);

    const int HitVoxel = Chunk->GetLocalVoxelOnly(LocalHitVoxelLocation);
    if (HitVoxel == ECommonVoxels::Null || HitVoxel == ECommonVoxels::Air)
    {
        LOG_WARNING(LogWorldChar, "Hit Voxel is not valid.")
        return;
    }

    Chunk->ModifySingleVoxel(LocalHitVoxelLocation, ECommonVoxels::Air);

    check( this->GetWorld()->GetSubsystem<UEntityWorldSubsystem>() )
    this->GetWorld()->GetSubsystem<UEntityWorldSubsystem>()->CreateDrop(
        FAccumulated(HitVoxel),
        HitResult.Location + HitResult.Normal,
        UEntityWorldSubsystem::GetRandomForceVector(-1.0f, 1.0f, 0.0f, 1.0f),
        FMath::FRandRange(1000.0f, 150000.0f)
    );

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
    /* This is kinda sketchy as it does not work around the x|y|z == 0 borders. */
    WorldTargetVoxelLocation    = FVector(WorldTargetVoxelLocation.X + 50.0f * FMath::Sign(WorldTargetVoxelLocation.X), WorldTargetVoxelLocation.Y + 50.0f * FMath::Sign(WorldTargetVoxelLocation.Y), WorldTargetVoxelLocation.Z + 50.0f * FMath::Sign(WorldTargetVoxelLocation.Z));

    /* Only checking ourselves currently maybe we want to do some more checks before sending the RPC. */
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
         * In the future, we should check with that.
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
    /* This is kinda sketchy as it does not work around the x|y|z == 0 borders. */
    WorldTargetVoxelLocation    = FVector(WorldTargetVoxelLocation.X + 50.0f * FMath::Sign(WorldTargetVoxelLocation.X), WorldTargetVoxelLocation.Y + 50.0f * FMath::Sign(WorldTargetVoxelLocation.Y), WorldTargetVoxelLocation.Z + 50.0f * FMath::Sign(WorldTargetVoxelLocation.Z));

    /* Only checking ourselves currently maybe we want to do some more checks before sending the RPC. */
    if (FVector::Dist(this->GetTorsoLocation(), WorldTargetVoxelLocation) < 100.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldCharacter::OnSecondary_ServerRPC: Distance is less than 100."))
        return;
    }

    if (this->Inventory[this->SelectedQuickSlotIndex].Content == Accumulated::Null)
    {
        return;
    }

    TargetedChunk->ModifySingleVoxel(LocalTargetVoxelLocation, this->Inventory[this->SelectedQuickSlotIndex].Content.AccumulatedIndex);

    bool bSuccess = false;
    this->Inventory[this->SelectedQuickSlotIndex].Content.SafeAddAmount(-1, bSuccess);
    DEFAULT_SLOT_SAFE_ADD_POST_BEHAVIOR(bSuccess, this->SelectedQuickSlotIndex, this->Inventory)
    MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, Inventory, this)
    /* Dirty pushes are not called on standalone or listen servers. */
    if (this->IsLocallyControlled())
    {
        this->OnRep_Inventory();
    }

    return;
}

/** Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void AWorldCharacter::OnToggleInventory(const FInputActionValue& Value)
{
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_INPUT_SUBSYSTEM)
    {
        if (Subsystem->HasMappingContext(this->IMCInventory))
        {
            HEAD_UP_DISPLAY->ToggleCharacterInventory(true);

            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(this->IMCFoot, 0);

            return;
        }

        HEAD_UP_DISPLAY->ToggleCharacterInventory(false);

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(this->IMCInventory, 0);

        return;
    }

    LOG_FATAL(LogWorldChar, "Enhanced Input subsystem is not available.")

    return;
}

void AWorldCharacter::OnDropAccumulated(const FInputActionValue& Value)
{
    /*
     * We simply ignore all predictions from the client and let the server (that is also us) handle the drop.
     */
    if (UNetworkStatics::IsSafeStandalone(this))
    {
        if (this->OnDropAccumulated_ServerRPC_Validate(Value))
        {
            this->OnDropAccumulated_ServerRPC_Implementation(Value);
        }

        this->OnInventoryChanged_ClientDelegate.Broadcast();

        return;
    }

    if (const UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_INPUT_SUBSYSTEM)
    {
        if (Subsystem->HasMappingContext(this->IMCInventory))
        {
            return;
        }

        bool bContentsChanged = false;
        this->Inventory[this->SelectedQuickSlotIndex].OnDrop(this, bContentsChanged, UNetworkStatics::IsSafeListenServer(this));

        if (bContentsChanged == false)
        {
            return;
        }

        /*
         * We update the UI immediately to give the player a more responsive UI.
         * But let all other processes be handled by the server, for example, spawning the accumulated
         * item as a drop in the UWorld.
         */

        this->OnInventoryChanged_ClientDelegate.Broadcast();

        if (UNetworkStatics::IsSafeListenServer(this))
        {
            return;
        }

        /*
         * Only sent the RPC to the server if we are not the server.
         * We already handled the drop on the server side.
         */
        this->OnDropAccumulated_ServerRPC(Value);

        return;
    }

    LOG_FATAL(LogWorldChar, "Enhanced Input subsystem is not available.")

    return;
}

bool AWorldCharacter::OnDropAccumulated_ServerRPC_Validate(const FInputActionValue& Value)
{
    LOG_VERBOSE(LogWorldChar, "Called.")

    bool bContentsChanged = false;
    this->Inventory[this->SelectedQuickSlotIndex].OnDrop(this, bContentsChanged, true);

    return bContentsChanged;
}

void AWorldCharacter::OnDropAccumulated_ServerRPC_Implementation(const FInputActionValue& Value)
{
    MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, Inventory, this)
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

void AWorldCharacter::OnQuickSlot0(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_0);
}

void AWorldCharacter::OnQuickSlot1(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_1);
}

void AWorldCharacter::OnQuickSlot2(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_2);
}

void AWorldCharacter::OnQuickSlot3(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_3);
}

void AWorldCharacter::OnQuickSlot4(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_4);
}

void AWorldCharacter::OnQuickSlot5(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_5);
}

void AWorldCharacter::OnQuickSlot6(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_6);
}

void AWorldCharacter::OnQuickSlot7(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_7);
}

void AWorldCharacter::OnQuickSlot8(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_8);
}

void AWorldCharacter::OnQuickSlot9(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_9);
}

void AWorldCharacter::OnQuickSlotBitwise(const FInputActionValue& Value)
{
    if (FMath::IsNearlyZero(Value.Get<FVector2D>().X))
    {
        return;
    }

    if (Value.Get<FVector>().X < 0.0)
    {
        this->OnQuickSlot((this->SelectedQuickSlotIndex + 1) % (QUICK_SLOT_MAX + 1));
        return;
    }

    this->OnQuickSlot(this->SelectedQuickSlotIndex - 1 < QUICK_SLOT_MIN ? QUICK_SLOT_MAX : this->SelectedQuickSlotIndex - 1);

    return;
}

void AWorldCharacter::OnQuickSlot(const int Slot)
{
    if (this->SelectedQuickSlotIndex == Slot)
    {
        return;
    }

    /*
     * We predict the outcome to allow the client to have a more responsive UI.
     * We do not want to wait for the server to respond to the client.
     * The replicated quick slot is just for other clients to see what the player is currently using.
     */
    this->SelectedQuickSlotIndex = Slot;

    this->OnQuickSlotLocationChanged_ClientDelegate.Broadcast();

    this->SetSelectedQuickSlotIndex_ServerRPC(Slot);

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

/** Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void AWorldCharacter::OnToggleDebugScreen(const FInputActionValue& Value)
{
    CHECKED_HEAD_UP_DISPLAY->ToggleDebugScreen();

    /*
     * Just temporary. We probably want to have a better way to handle this in the future.
     */
    check( GEngine )
    check( this->GetWorld() )
    GEngine->Exec(this->GetWorld(), TEXT("j.ShowChunkBorders"));

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
        check( this->IMCInventory )

        check( this->IAJump )
        check( this->IALook )
        check( this->IAMove )

        check( this->IAPrimary )
        check( this->IASecondary )

        check( this->IAToggleInventory )
        check( this->IADropAccumulated )
        check( this->IAQuickSlot0 )
        check( this->IAQuickSlot1 )
        check( this->IAQuickSlot2 )
        check( this->IAQuickSlot3 )
        check( this->IAQuickSlot4 )
        check( this->IAQuickSlot5 )
        check( this->IAQuickSlot6 )
        check( this->IAQuickSlot7 )
        check( this->IAQuickSlot8 )
        check( this->IAQuickSlot9 )
        check( this->IAQuickSlotBitwise )

        check( this->IAToggleEscapeMenu )
        check( this->IAToggleChatMenu )
        check( this->IAToggleDebugScreen )

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(this->IMCFoot, 0);

        UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
        check( EnhancedInputComponent )

        EnhancedInputComponent->BindAction(this->IAJump, ETriggerEvent::Triggered, this, &AWorldCharacter::OnTriggerJump);
        EnhancedInputComponent->BindAction(this->IAJump, ETriggerEvent::Completed, this, &AWorldCharacter::OnCompleteJump);
        EnhancedInputComponent->BindAction(this->IALook, ETriggerEvent::Triggered, this, &AWorldCharacter::OnLook);
        EnhancedInputComponent->BindAction(this->IAMove, ETriggerEvent::Triggered, this, &AWorldCharacter::OnMove);

        EnhancedInputComponent->BindAction(this->IAPrimary, ETriggerEvent::Started, this, &AWorldCharacter::OnPrimary);
        EnhancedInputComponent->BindAction(this->IASecondary, ETriggerEvent::Started, this, &AWorldCharacter::OnSecondary);

        EnhancedInputComponent->BindAction(this->IAToggleInventory, ETriggerEvent::Started, this, &AWorldCharacter::OnToggleInventory);
        EnhancedInputComponent->BindAction(this->IADropAccumulated, ETriggerEvent::Started, this, &AWorldCharacter::OnDropAccumulated);
        EnhancedInputComponent->BindAction(this->IAQuickSlot0, ETriggerEvent::Started, this, &AWorldCharacter::OnQuickSlot0);
        EnhancedInputComponent->BindAction(this->IAQuickSlot1, ETriggerEvent::Started, this, &AWorldCharacter::OnQuickSlot1);
        EnhancedInputComponent->BindAction(this->IAQuickSlot2, ETriggerEvent::Started, this, &AWorldCharacter::OnQuickSlot2);
        EnhancedInputComponent->BindAction(this->IAQuickSlot3, ETriggerEvent::Started, this, &AWorldCharacter::OnQuickSlot3);
        EnhancedInputComponent->BindAction(this->IAQuickSlot4, ETriggerEvent::Started, this, &AWorldCharacter::OnQuickSlot4);
        EnhancedInputComponent->BindAction(this->IAQuickSlot5, ETriggerEvent::Started, this, &AWorldCharacter::OnQuickSlot5);
        EnhancedInputComponent->BindAction(this->IAQuickSlot6, ETriggerEvent::Started, this, &AWorldCharacter::OnQuickSlot6);
        EnhancedInputComponent->BindAction(this->IAQuickSlot7, ETriggerEvent::Started, this, &AWorldCharacter::OnQuickSlot7);
        EnhancedInputComponent->BindAction(this->IAQuickSlot8, ETriggerEvent::Started, this, &AWorldCharacter::OnQuickSlot8);
        EnhancedInputComponent->BindAction(this->IAQuickSlot9, ETriggerEvent::Started, this, &AWorldCharacter::OnQuickSlot9);
        EnhancedInputComponent->BindAction(this->IAQuickSlotBitwise, ETriggerEvent::Triggered, this, &AWorldCharacter::OnQuickSlotBitwise);

        EnhancedInputComponent->BindAction(this->IAToggleEscapeMenu, ETriggerEvent::Started, this, &AWorldCharacter::OnToggleEscapeMenu);
        EnhancedInputComponent->BindAction(this->IAToggleChatMenu, ETriggerEvent::Started, this, &AWorldCharacter::OnToggleChatMenu);
        EnhancedInputComponent->BindAction(this->IAToggleDebugScreen, ETriggerEvent::Started, this, &AWorldCharacter::OnToggleDebugScreen);
    }

    return;
}

void AWorldCharacter::OnRep_Inventory(void)
{
    LOG_VERBOSE(LogWorldChar, "Called.")

    this->OnInventoryChanged_ClientDelegate.Broadcast();

    return;
}

bool AWorldCharacter::OnInventorySlotPrimaryClicked_ServerRPC_Validate(const int Slot)
{
    LOG_VERBOSE(LogWorldChar, "Called.")

    bool bContentsChanged = false;
    this->Inventory[Slot].OnPrimaryClicked(this, bContentsChanged, true);

    if (bContentsChanged == false)
    {
        /*
         * Clients must always only send UI click input events to the server if the contents of the slot actually
         * are changed.
         * That is why we disconnect the client from the server here.
         */

        LOG_ERROR(LogWorldChar, "Contents did not change. Faulty input: %s %d.", *this->GetName(), Slot)
        return false;
    }

    return true;
}

void AWorldCharacter::OnInventorySlotPrimaryClicked_ServerRPC_Implementation(const int Slot)
{
    /*
     * See the RPC Validate method for the actual logic.
     */

    LOG_VERBOSE(LogWorldChar, "Called.")

    MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, Inventory, this)

    return;
}

bool AWorldCharacter::OnInventorySlotSecondaryClicked_ServerRPC_Validate(const int Slot)
{
    LOG_VERBOSE(LogWorldChar, "Called.")

    bool bContentsChanged = false;
    this->Inventory[Slot].OnSecondaryClicked(this, bContentsChanged, true);

    if (bContentsChanged == false)
    {
        /*
         * Clients must always only send UI click input events to the server if the contents of the slot actually
         * are changed.
         * That is why we disconnect the client from the server here.
         */

        LOG_ERROR(LogWorldChar, "Contents did not change. Faulty input: %s %d.", *this->GetName(), Slot)
        return false;
    }

    return true;
}

void AWorldCharacter::OnInventorySlotSecondaryClicked_ServerRPC_Implementation(const int Slot)
{
    /*
     * See the RPC Validate method for the actual logic.
     */

    LOG_VERBOSE(LogWorldChar, "Called.")

    MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, Inventory, this)

    return;
}

void AWorldCharacter::OnRep_CursorHand(void) const
{
    LOG_VERBOSE(LogWorldChar, "Called.")
    this->OnInventoryChanged_ClientDelegate.Broadcast();
    return;
}

void AWorldCharacter::OnRep_SelectedQuickSlotIndex(void)
{
    if (this->IsLocallyControlled() == false)
    {
        /*
         * Here than update the mesh of the character to show the selected quick slot.
         */
        return;
    }

    this->OnQuickSlotLocationChanged_ClientDelegate.Broadcast();

    return;
}

void AWorldCharacter::SetSelectedQuickSlotIndex_ServerRPC_Implementation(const int Slot)
{
#if WITH_ENGINE
    if (UNetworkStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogWorldChar, "Called not on a server. Disallowed.")
        return;
    }
#endif /* WITH_ENGINE */

    if (this->SelectedQuickSlotIndex == Slot)
    {
        /*
         * We predict the outcome already in the input method directly to allow for a more responsive UI and a smoother
         * experience if the latency to the server is high. This of course is going to update the same variable if the
         * server is a listen server and this character is locally controlled.
         * See the AWorldCharacter#OnQuickSlot(int) method for a detailed explanation.
         */
        if ((UNetworkStatics::IsSafeListenServer(this) && this->IsLocallyControlled()) || UNetworkStatics::IsSafeStandalone(this))
        {
            LOG_VERY_VERBOSE(LogWorldChar, "%s switched quick slot to %d.", *this->GetName(), Slot)
            MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, SelectedQuickSlotIndex, this)
            return;
        }

        /*
         * Happens if to many inputs are sent to the server.
         * Can we prevent the spam of inputs? A future problem.
         */
        LOG_WARNING(LogWorldChar, "Selected Quick Slot Index did not change. Faulty input: %s %d.", *this->GetName(), Slot)
        return;
    }

    LOG_VERY_VERBOSE(LogWorldChar, "%s switched quick slot to %d.", *this->GetName(), Slot)

    this->SelectedQuickSlotIndex = Slot;
    MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, SelectedQuickSlotIndex, this)

    return;
}

void AWorldCharacter::AddToInventoryAtSlot(const int Slot, const int Amount)
{
    if (UNetworkStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogWorldChar, "Called not on a server. Disallowed.")
        return;
    }

    bool bSuccess;
    this->Inventory[Slot].Content.SafeAddAmount(Amount, bSuccess);

    DEFAULT_SLOT_SAFE_ADD_POST_BEHAVIOR(bSuccess, Slot, this->Inventory)

    MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, Inventory, this)

    /*
     * Dirty pushes are not called here. This applies for standalone and listen servers.
     * See PushModel.h for more information.
     */
    if (this->IsLocallyControlled())
    {
        this->OnRep_Inventory();
    }

    return;
}

void AWorldCharacter::SetInventoryAtSlot(const int Slot, const FAccumulated& Accumulated)
{
    if (UNetworkStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogWorldChar, "Called not on a server. Disallowed.")
        return;
    }

    this->Inventory[Slot].Content = Accumulated;

    MARK_PROPERTY_DIRTY_FROM_NAME(AWorldCharacter, Inventory, this)

    /*
     * Dirty pushes are not called here. This applies for standalone and listen servers.
     * See PushModel.h for more information.
     */
    if (this->IsLocallyControlled())
    {
        this->OnRep_Inventory();
    }

    return;
}

void AWorldCharacter::AskForInventoryChangeDelegateBroadcast(void) const
{
    if (this->IsLocallyControlled() == false)
    {
        LOG_FATAL(LogWorldChar, "Called not on locally controlled character. Disallowed.")
        return;
    }

    this->OnInventoryChanged_ClientDelegate.Broadcast();

    return;
}

FDelegateHandle AWorldCharacter::SubscribeToInventoryChanged(const FOnClientCharacterPropertyChangedEventSignature::FDelegate& Delegate)
{
    if (this->IsLocallyControlled() == false)
    {
        LOG_FATAL(LogWorldChar, "Called not on locally controlled character. Disallowed.")
        return FDelegateHandle();
    }

    return this->OnInventoryChanged_ClientDelegate.Add(Delegate);
}

bool AWorldCharacter::UnsubscribeFromInventoryChanged(const FDelegateHandle& Handle)
{
    return this->OnInventoryChanged_ClientDelegate.Remove(Handle);
}

FDelegateHandle AWorldCharacter::SubscribeToQuickSlotLocationChanged(const FOnClientCharacterPropertyChangedEventSignature::FDelegate& Delegate)
{
    if (this->IsLocallyControlled() == false)
    {
        LOG_FATAL(LogWorldChar, "Called not on locally controlled character. Disallowed.")
        return FDelegateHandle();
    }

    return this->OnQuickSlotLocationChanged_ClientDelegate.Add(Delegate);
}

bool AWorldCharacter::AddToInventory(const FAccumulated& Accumulated)
{
    if (UNetworkStatics::IsSafeServer(this) == false)
    {
        LOG_FATAL(LogWorldChar, "Called not on a server. Disallowed.")
        return false;
    }

    /* Check if there is an existing accumulated item already in the character's inventory. */
    for (int i = 0; i < this->Inventory.Num(); ++i)
    {
        if (this->Inventory[i].Content.AccumulatedIndex == Accumulated.AccumulatedIndex)
        {
            this->AddToInventoryAtSlot(i, Accumulated.Amount);
            return true;
        }

        continue;
    }

    for (int i = 0; i < this->Inventory.Num(); ++i)
    {
        if (this->Inventory[i].Content != Accumulated::Null)
        {
            continue;
        }

        this->SetInventoryAtSlot(i, Accumulated);

        return true;
    }

    return false;
}

void AWorldCharacter::GetTargetedVoxel(
    ACommonChunk*& OutChunk,
    FVector& OutWorldHitLocation,
    FVector_NetQuantizeNormal& OutWorldNormalHitLocation,
    FIntVector& OutLocalHitVoxelLocation,
    const bool bUseRemotePitch,
    const float UnrealReach
) const
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
        FVector FixedVector = HitResult.Location;
        if (FMath::IsNearlyZero(FixedVector.X, UE_DOUBLE_KINDA_SMALL_NUMBER))
        {
            FixedVector.X = 0.0;
        }

        if (FMath::IsNearlyZero(FixedVector.Y, UE_DOUBLE_KINDA_SMALL_NUMBER))
        {
            FixedVector.Y = 0.0;
        }

        if (FMath::IsNearlyZero(FixedVector.Z, UE_DOUBLE_KINDA_SMALL_NUMBER))
        {
            FixedVector.Z = 0.0;
        }

        FixedVector -= HitResult.Normal;

        OutWorldHitLocation =  FixedVector;
        OutWorldNormalHitLocation = HitResult.Normal;
        OutLocalHitVoxelLocation = ACommonChunk::WorldToLocalVoxelLocation(OutWorldHitLocation);

        return;
    }

    return;
}

#undef PLAYER_CONTROLLER
#undef HEAD_UP_DISPLAY
#undef CHECKED_HEAD_UP_DISPLAY
#undef ENHANCED_INPUT_SUBSYSTEM
