// © 2023 mzoesch. All rights reserved.

#include "Core/CH_Master.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Core/GM_Master.h"
#include "Core/GI_Master.h"
#include "World/Chunk.h"
#include "World/ChunkWorld.h"
#include "HUD/HUD_Master.h"
#include "HUD/OSD/Crosshair.h"
#include "Lib/Cuboid.h"
#include "Core/PC_Master.h"
#include "Lib/PrescriptionSeeker.h"
#include "World/EntityMaster.h"
#include "World/WorldVoxel.h"
#include "Entity/CharacterReach.h"
#include "World/Voxels/Voxel.h"

#define UIL_LOG(Verbosity, Format, ...)     UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define HUD                                 Cast<AHUD_Master>(this->GetWorld()->GetFirstPlayerController()->GetHUD())
#define ENHANCED_SUBSYSTEM                  ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Cast<APlayerController>(this->GetWorld()->GetFirstPlayerController())->GetLocalPlayer())
#define PLAYER_CONTROLLER                   Cast<APC_Master>(this->GetWorld()->GetFirstPlayerController())
#define GAME_MODE                           Cast<AGM_Master>(UGameplayStatics::GetGameMode(this->GetWorld()))
#define GAME_INSTANCE                       Cast<UGI_Master>(UGameplayStatics::GetGameInstance(this->GetWorld()))
#define ENTITY_MASTER                       Cast<AEntityMaster>(UGameplayStatics::GetActorOfClass(this, AEntityMaster::StaticClass()))

ACH_Master::ACH_Master()
{
    this->PrimaryActorTick.bCanEverTick = true;

    this->GetCapsuleComponent()->InitCapsuleSize(40.0f, 90.0f);

    this->FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    this->FirstPersonCameraComponent->SetupAttachment(this->GetCapsuleComponent());
    this->FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f));
    this->FirstPersonCameraComponent->bUsePawnControlRotation = true;
    this->FirstPersonCameraComponent->SetFieldOfView( 70.0f /* 120.0f */ );

    this->ItemPreview       = nullptr;
    this->CharacterReach    = nullptr;
    
    this->GetCharacterMovement()->GravityScale                  = 2.0f;
    this->GetCharacterMovement()->JumpZVelocity                 = 700.0f;
    this->GetCharacterMovement()->AirControl                    = 2.0f;
    this->GetCharacterMovement()->bUseFlatBaseForFloorChecks    = true;

    this->CurrentDurationSameVoxelIsMined       = 0.0f;
    this->CurrentlyMiningLocalVoxelLocation     = FIntVector::ZeroValue;
    
    this->SelectedQuickSlotIndex = 0;
    this->CursorHand = FAccumulated::NullAccumulated;
    this->Inventory.SetNum(ACH_Master::InventoryStartSize, false);
    this->InventoryCrafter.SetNum(ACH_Master::InventoryCrafterSize, false);
    
    return;
}

void ACH_Master::BeginPlay(void)
{
    Super::BeginPlay();

    /* Array sizes must always be initialized first. Many initializing methods search for specific indices in these arrays. */
    this->Inventory.SetNum(ACH_Master::InventoryStartSize, false);
    this->InventoryCrafter.SetNum(ACH_Master::InventoryCrafterSize, false);

    this->ItemPreview = this->GetWorld()->SpawnActor<ACuboid>(ACuboid::StaticClass(), FTransform(),FActorSpawnParameters());
    
    this->ItemPreview->SetUsePOVMaterial(true);
    this->ItemPreview->AttachToComponent(this->FirstPersonCameraComponent, FAttachmentTransformRules::KeepRelativeTransform);
    this->UpdateItemPreview();

    check( this->CharacterReachClass )
    this->CharacterReach = this->GetWorld()->SpawnActor<ACharacterReach>(this->CharacterReachClass, FTransform(), FActorSpawnParameters());
    this->CharacterReach->AttachToComponent(this->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);

    this->CurrentDurationSameVoxelIsMined       = 0.0f;
    this->CurrentlyMiningLocalVoxelLocation     = FIntVector::ZeroValue;

    this->SelectedQuickSlotIndex = 0;
    this->CursorHand = FAccumulated::NullAccumulated;

    this->AddToInventory(FAccumulated(GAME_INSTANCE->GetAccumulatedByName("StoneVoxel").Accumulated, 64), false);
    this->AddToInventory(FAccumulated(GAME_INSTANCE->GetAccumulatedByName("DirtVoxel").Accumulated, 64), false);
    this->AddToInventory(FAccumulated(GAME_INSTANCE->GetAccumulatedByName("GrassVoxel").Accumulated, 64), false);
    this->AddToInventory(FAccumulated(GAME_INSTANCE->GetAccumulatedByName("OakLogVoxel").Accumulated, 64), false);
    this->AddToInventory(FAccumulated(GAME_INSTANCE->GetAccumulatedByName("OakPlanksVoxel").Accumulated, 64), false);
    this->AddToInventory(FAccumulated(GAME_INSTANCE->GetAccumulatedByName("CraftingTableVoxel").Accumulated, 64), false);
    this->AddToInventory(FAccumulated(GAME_INSTANCE->GetAccumulatedByName("Stick").Accumulated, 64), false);
    this->AddToInventory(FAccumulated(GAME_INSTANCE->GetAccumulatedByName("WoodenSword").Accumulated, 64), true);
    
    return;
}

void ACH_Master::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    AChunk*                     TargetedChunk;
    FVector                     TargetedWorldHitLocation;
    FVector_NetQuantizeNormal   TargetedWorldNormalHitLocation;
    FIntVector                  TargetedLocalHitVoxelLocation;
    this->GetTargetedVoxel(TargetedChunk, TargetedWorldHitLocation, TargetedWorldNormalHitLocation, TargetedLocalHitVoxelLocation, this->GetCharacterReach());

    if (TargetedChunk == nullptr)
    {
        HUD->UpdateCrosshair(ECrosshairState::ECS_Default);
        this->CharacterReach->OnUpdate(false);
    }
    else
    {
        HUD->UpdateCrosshair(ECrosshairState::ECS_Interaction);
        this->CharacterReach->OnUpdate(TargetedWorldHitLocation);
    }
    
    return;
}

void ACH_Master::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    check( PLAYER_CONTROLLER )

#if WITH_EDITOR
    if (Cast<UEnhancedPlayerInput>(PLAYER_CONTROLLER->PlayerInput) == nullptr)
    {
        UIL_LOG(Error, TEXT("ACH_Master::SetupPlayerInputComponent: Player Controller is not using the Enhanced Input subsystem."))
        return;
    }
#endif /* WITH_EDITOR */
    
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_SUBSYSTEM)
    {
        check( Subsystem )

        check( this->IMCFoot      )
        check( this->IMCContainer )

        check( this->IAMove )
        check( this->IALook )
        check( this->IAJump )

        check( this->IAPrimary         )
        check( this->IASecondary       )
        check( this->IADropAccumulated )

        check( this->IAToggleInventory  )
        check( this->IAQuickSlot0       )
        check( this->IAQuickSlot1       )
        check( this->IAQuickSlot2       )
        check( this->IAQuickSlot3       )
        check( this->IAQuickSlot4       )
        check( this->IAQuickSlot5       )
        check( this->IAQuickSlot6       )
        check( this->IAQuickSlot7       )
        check( this->IAQuickSlot8       )
        check( this->IAQuickSlot9       )
        check( this->IAQuickSlotBitwise )

        check( this->IADebugScreen )
        
        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(this->IMCFoot, 0);
        UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
        
        /* Movement */
        EnhancedInputComponent->BindAction(this->IAMove, ETriggerEvent::Triggered, this, &ACH_Master::OnMove);
        EnhancedInputComponent->BindAction(this->IALook, ETriggerEvent::Triggered, this, &ACH_Master::OnLook);
        EnhancedInputComponent->BindAction(this->IAJump, ETriggerEvent::Triggered, this, &ACH_Master::OnTriggerJump);
        EnhancedInputComponent->BindAction(this->IAJump, ETriggerEvent::Completed, this, &ACH_Master::OnCompleteJump);

        /* Interaction */
        EnhancedInputComponent->BindAction(this->IAPrimary, ETriggerEvent::Triggered, this, &ACH_Master::OnPrimary);
        EnhancedInputComponent->BindAction(this->IAPrimary, ETriggerEvent::Completed, this, &ACH_Master::OnPrimaryCompleted);
        EnhancedInputComponent->BindAction(this->IASecondary, ETriggerEvent::Started, this, &ACH_Master::OnSecondary);
        EnhancedInputComponent->BindAction(this->IADropAccumulated, ETriggerEvent::Started, this, &ACH_Master::OnDropAccumulated);

        /* Inventory */
        EnhancedInputComponent->BindAction(this->IAToggleInventory, ETriggerEvent::Started, this, &ACH_Master::OnInventoryToggle);
        EnhancedInputComponent->BindAction(this->IAQuickSlot0, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot0);
        EnhancedInputComponent->BindAction(this->IAQuickSlot1, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot1);
        EnhancedInputComponent->BindAction(this->IAQuickSlot2, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot2);
        EnhancedInputComponent->BindAction(this->IAQuickSlot3, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot3);
        EnhancedInputComponent->BindAction(this->IAQuickSlot4, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot4);
        EnhancedInputComponent->BindAction(this->IAQuickSlot5, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot5);
        EnhancedInputComponent->BindAction(this->IAQuickSlot6, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot6);
        EnhancedInputComponent->BindAction(this->IAQuickSlot7, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot7);
        EnhancedInputComponent->BindAction(this->IAQuickSlot8, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot8);
        EnhancedInputComponent->BindAction(this->IAQuickSlot9, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot9);
        EnhancedInputComponent->BindAction(this->IAQuickSlotBitwise, ETriggerEvent::Triggered, this, &ACH_Master::OnQuickSlotBitwise);

        /* MISC */
        EnhancedInputComponent->BindAction(this->IADebugScreen, ETriggerEvent::Started, this, &ACH_Master::OnDebugScreenToggle);
    }

    return;
}

#pragma region Input Actions

void ACH_Master::OnMove(const FInputActionValue& Value)
{
    this->AddMovementInput( this->GetActorForwardVector(), Value.Get<FVector2D>().Y );
    this->AddMovementInput( this->GetActorRightVector(),   Value.Get<FVector2D>().X );
    return;
}

void ACH_Master::OnLook(const FInputActionValue& Value)
{
    this->AddControllerYawInput(   Value.Get<FVector2D>().X *  0.2f );
    this->AddControllerPitchInput( Value.Get<FVector2D>().Y * -0.2f );
    return;
}

void ACH_Master::OnTriggerJump(const FInputActionValue& Value)
{
    Super::Jump();
    return;
}

void ACH_Master::OnCompleteJump(const FInputActionValue& Value)
{
    Super::StopJumping();
    return;
}

void ACH_Master::OnPrimary(const FInputActionValue& Value)
{
    const FTransform                TraceStart      = this->GetFirstPersonTraceStart();
    const FVector                   TraceEnd        = TraceStart.GetLocation() + (TraceStart.GetRotation().Vector() * this->GetCharacterReach());
    const FCollisionQueryParams     TraceParams     = FCollisionQueryParams(FName(TEXT("")), false, this->GetOwner());

    FHitResult HitResult;
    this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart.GetLocation(), TraceEnd, ECollisionChannel::ECC_Visibility, TraceParams);

    if (HitResult.GetActor() == nullptr)
    {
        return;
    }

    if (HitResult.GetActor()->IsA(AChunk::StaticClass()) == false)
    {
        return;
    }
	
    AChunk*         Chunk                   = CastChecked<AChunk>(HitResult.GetActor());
    const FVector   WorldHitLocation        = HitResult.Location - HitResult.Normal;
    FIntVector      LocalHitVoxelLocation   = AChunkWorld::WorldToLocalVoxelPosition(WorldHitLocation);

    const int HitVoxel = Chunk->GetVoxel(LocalHitVoxelLocation);
    if (HitVoxel == EWorldVoxel::WV_Null || HitVoxel == EWorldVoxel::WV_Air)
    {
        return;
    }

    if (this->CurrentlyMiningLocalVoxelLocation != LocalHitVoxelLocation)
    {
        this->CurrentlyMiningLocalVoxelLocation = LocalHitVoxelLocation;
        this->CurrentDurationSameVoxelIsMined = 0.0;
    }
    else
    {
        this->CurrentDurationSameVoxelIsMined += this->GetWorld()->GetDeltaSeconds();
    }

    if (this->CurrentDurationSameVoxelIsMined < 0.5)
    {
        this->CharacterReach->UpdateMaterial(this->CurrentDurationSameVoxelIsMined / 0.5f);
        return;
    }
    
    Chunk->ModifyVoxel(LocalHitVoxelLocation, EWorldVoxel::WV_Air);

    ENTITY_MASTER->CreateDrop(FAccumulated(HitVoxel), HitResult.Location + HitResult.Normal, AEntityMaster::GetRandomForceVector(-1.0f, 1.0f, 0.0f, 1.0f), FMath::FRandRange(1000.0f, 150000.0f));
    
    return;
}

void ACH_Master::OnPrimaryCompleted(const FInputActionValue& Value)
{
    this->CurrentlyMiningLocalVoxelLocation     = FIntVector::ZeroValue;
    this->CurrentDurationSameVoxelIsMined       = 0.0;
    this->CharacterReach->UpdateMaterial(0.0f);
    return;
}

void ACH_Master::OnSecondary(const FInputActionValue& Value)
{
    AChunk*                     TargetedChunk;
    FVector                     TargetedWorldHitLocation;
    FVector_NetQuantizeNormal   TargetedWorldNormalHitLocation;
    FIntVector                  TargetedLocalHitVoxelLocation;
    this->GetTargetedVoxel(TargetedChunk, TargetedWorldHitLocation, TargetedWorldNormalHitLocation, TargetedLocalHitVoxelLocation, this->GetCharacterReach());

    if (TargetedChunk == nullptr)
    {
        /* TODO We may still trigger the secondary event for the accumulated quick slot item, if it does not require a UWorld voxel. */
        UIL_LOG(Log, TEXT("ACH_Master::OnSecondary: No targeted chunk found."))
        return;
    }

    if (FVoxelMask TargetedVoxelMask; TargetedChunk->HasCustomSecondaryCharacterEventVoxel(TargetedLocalHitVoxelLocation, TargetedVoxelMask))
    {
        bool bConsumed;
        TargetedVoxelMask.VoxelClass.GetInterface()->OnCustomSecondaryCharacterEvent(this, bConsumed);
        if (bConsumed)
        {
            return;
        }
    }

    if (this->Inventory[this->SelectedQuickSlotIndex].Content.IsVoxel() == false)
    {
        /* TODO Trigger custom secondary event for the accumulated quick slot item if existing. */
        return;
    }
    
    FVector             WorldTargetVoxelLocation    = TargetedWorldHitLocation + TargetedWorldNormalHitLocation * 50.0f;
    const FIntVector    LocalTargetVoxelLocation    = FIntVector(TargetedWorldNormalHitLocation + FVector(TargetedLocalHitVoxelLocation));
                        WorldTargetVoxelLocation    = FVector(WorldTargetVoxelLocation.X - FMath::Fmod(WorldTargetVoxelLocation.X, 100.0f), WorldTargetVoxelLocation.Y - FMath::Fmod(WorldTargetVoxelLocation.Y, 100.0f), WorldTargetVoxelLocation.Z - FMath::Fmod(WorldTargetVoxelLocation.Z, 100.0f));
                        /* This is kinda sketchy as it does not work around the x|y|z == 0 border. */
                        WorldTargetVoxelLocation    = FVector(WorldTargetVoxelLocation.X + 50.0f * FMath::Sign(WorldTargetVoxelLocation.X), WorldTargetVoxelLocation.Y + 50.0f * FMath::Sign(WorldTargetVoxelLocation.Y), WorldTargetVoxelLocation.Z + 50.0f * FMath::Sign(WorldTargetVoxelLocation.Z));
    
    /* TODO We ofc have to check more than just the calling AActor. */
    if (FVector::Dist(this->GetTorsoLocation(), WorldTargetVoxelLocation) < 100.0f)
    {
        return;
    }

    if (this->Inventory[this->SelectedQuickSlotIndex].Content.Accumulated != EWorldVoxel::WV_Null && this->Inventory[this->SelectedQuickSlotIndex].Content.Accumulated != EWorldVoxel::WV_Air)
    {
        TargetedChunk->ModifyVoxel(LocalTargetVoxelLocation, this->Inventory[this->SelectedQuickSlotIndex].Content.Accumulated);
        this->AddToInventoryAtSlot(this->SelectedQuickSlotIndex, -1, true);
        return;
    }

    return;
}

void ACH_Master::OnDropAccumulated(const FInputActionValue& Value)
{
    if (this->IsAContainerVisible())
    {
        /* TODO Kinda sketchy here. We want to figure out the current pos of the cursor and drop the accumulated on that slot. */
        return;
    }

    bool bContentsChanged = false;
    this->Inventory[this->SelectedQuickSlotIndex].OnDrop(this, bContentsChanged);

    if (bContentsChanged)
    {
        HUD->RefreshCharacterHotbar();
        this->UpdateItemPreview();
    }
    
    return;
}

void ACH_Master::OnInventoryToggle(const FInputActionValue& Value)
{
    /*
     * As this is the direct method called from the input action binding, we sometimes have to misuse
     * these actions.
     *
     * A character may not open the inventory if an other container is already open. If this is the case
     * we have to close the other container first. The inventory will not be opened.
     */

    /* Another container is open. */
    if (HUD->IsContainerOpen() && HUD->GetCurrentOpenContainer() != this->InventoryContainerIdentifier)
    {
        PLAYER_CONTROLLER->TransitToContainerState(HUD->GetCurrentOpenContainer(), false);
        return;
    }
    
    if (this->InventoryContainerIdentifier.IsEmpty())
    {
        HUD->AddCharacterInventoryContainer(this->InventoryContainerIdentifier);
    }

    if (const UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_SUBSYSTEM)
    {
        if (Subsystem->HasMappingContext(this->IMCContainer))
        {
            PLAYER_CONTROLLER->TransitToContainerState(this->InventoryContainerIdentifier, false, false);
            return;
        }

        PLAYER_CONTROLLER->TransitToContainerState(this->InventoryContainerIdentifier, true);
        
        return;
    }

#if WITH_EDITOR
    UIL_LOG(Fatal, TEXT("ACH_Master::OnInventoryToggle: Enhanced Subsystem not found on local character."))
#endif /* WITH_EDITOR */
    
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

void ACH_Master::OnQuickSlot0(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_0);
    return;
}

void ACH_Master::OnQuickSlot1(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_1);
    return;
}

void ACH_Master::OnQuickSlot2(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_2);
    return;
}

void ACH_Master::OnQuickSlot3(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_3);
    return;
}

void ACH_Master::OnQuickSlot4(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_4);
    return;
}

void ACH_Master::OnQuickSlot5(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_5);
    return;
}

void ACH_Master::OnQuickSlot6(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_6);
    return;
}

void ACH_Master::OnQuickSlot7(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_7);
    return;
}

void ACH_Master::OnQuickSlot8(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_8);
    return;
}

void ACH_Master::OnQuickSlot9(const FInputActionValue& Value)
{
    this->OnQuickSlot(QUICK_SLOT_9);
    return;
}

void ACH_Master::OnQuickSlotBitwise(const FInputActionValue& Value)
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

void ACH_Master::OnQuickSlot(const int Slot)
{
    this->SelectedQuickSlotIndex = Slot;
    HUD->RefreshCharacterHotbarSelector();
    this->UpdateItemPreview();
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
void ACH_Master::OnDebugScreenToggle(const FInputActionValue& Value)
{
    HUD->OnDebugScreenToggle();
    return;
}

#pragma endregion Input Actions

#pragma region Member Methods

void ACH_Master::UpdateItemPreview(void) const
{
    this->ItemPreview->SetActorRelativeTransform(
        this->GetSelectedQuickSlot().IsVoxel()
        ?
        FTransform(
            FRotator(180.0f, 40.0f, 0.0f),
            FVector(48.0f, 33.5f, -30.0),
            FVector(0.1f, 0.1f, 0.1f)
        )
        :
        FTransform(
            FRotator(65.0f, 0.0f, 90.0f),
            FVector(18.0f, 29.0f, -25.0f),
            FVector(0.1f, 0.1f, 0.1f)
        )
        
    );

    this->ItemPreview->GenerateMesh(this->GetSelectedQuickSlot().Accumulated);
    
    return;
}

void ACH_Master::TransitCharacterToContainerState(const bool bOpen) const
{
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_SUBSYSTEM)
    {
        if (bOpen)
        {
            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(this->IMCContainer, 0);
            return;
        }

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(this->IMCFoot, 0);
        
        return;
    }

#if WITH_EDITOR
    UIL_LOG(Error, TEXT("ACH_Master::TransitCharacterToContainerState: Enhanced Subsystem not found on local character."))
#else
    UIL_LOG(Fatal, TEXT("ACH_Master::TransitCharacterToContainerState: Enhanced Subsystem not found on local character."))
#endif /* WITH_EDITOR */
    
    return;
}

#pragma region Inventory Manipulation

bool ACH_Master::IsAContainerVisible(void) const
{
    if (const UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_SUBSYSTEM)
    {
        return Subsystem->HasMappingContext(this->IMCContainer);
    }

#if WITH_EDITOR
    UIL_LOG(Fatal, TEXT("ACH_Master::IsAContainerVisible: Enhanced Subsystem not found on local character."))
#endif /* WITH_EDITOR */
    
    return false;
}

void ACH_Master::ClearCursorHand(const bool bUpdateHUD)
{
    if (this->CursorHand == FAccumulated::NullAccumulated)
    {
        return;
    }

    const FAccumulated Temp     = this->CursorHand;
    this->CursorHand            = FAccumulated::NullAccumulated;

    if (this->AddToInventory(Temp, bUpdateHUD))
    {
        return;
    }

    /* TODO Drop the accumulated item. */
    UIL_LOG(Error, TEXT("ACH_Master::ClearCursorHand: Failed to add the cursor hand to the inventory. Drop logic is not implemented yet."))

    return;    
}

bool ACH_Master::AddToInventory(const FAccumulated Accumulated, const bool bUpdateHUD)
{
    /* Check if there is an existing accumulated item already in the characters inventory. */
    for (int i = 0; i < this->Inventory.Num(); i++)
    {
        if (this->Inventory[i].Content.Accumulated == Accumulated.Accumulated)
        {
            this->AddToInventoryAtSlot(i, Accumulated.Amount, bUpdateHUD);
            return true;
        }

        continue;
    }

    /* Add the new accumulated item to an empty inventory slot. */
    for (int i = 0; i < this->Inventory.Num(); i++)
    {
        if (this->Inventory[i].Content != FAccumulated::NullAccumulated)
        {
            continue;
        }
        
        this->Inventory[i].Content = Accumulated;

        if (bUpdateHUD)
        {
            HUD->RefreshCurrentOpenedContainerWithCharacterHotbar();
            this->UpdateItemPreview();
        }
        
        return true;
    }
    
    return false;
}

void ACH_Master::OnInventorySlotClicked(const int Slot, const bool bUpdateHUD)
{
    bool bContentsChanged = false;
    this->Inventory[Slot].OnClicked(this, bContentsChanged);

    if (bContentsChanged == false)
    {
        return;
    }

    if (bUpdateHUD)
    {
        HUD->RefreshCurrentOpenedContainerWithCharacterHotbar();
    }
    
    return;
}

void ACH_Master::OnInventorySlotSecondaryClicked(const int Slot, const bool bUpdateHUD)
{
    bool bContentChanged = false;
    this->Inventory[Slot].OnSecondaryClicked(this, bContentChanged);

    if (bContentChanged == false)
    {
        return;
    }

    if (bUpdateHUD)
    {
        HUD->RefreshCurrentOpenedContainerWithCharacterHotbar();
    }
    
    return;
}

void ACH_Master::OnInventoryCrafterSlotClicked(const int Slot, const bool bUpdateHUD)
{
    bool bContentsChanged = false;
    this->InventoryCrafter[Slot].OnClicked(this, bContentsChanged);

    if (bContentsChanged == false)
    {
        return;
    }

    if (bUpdateHUD)
    {
        HUD->RefreshCurrentOpenedContainerWithCharacterHotbar();
    }
    
    return;
}

void ACH_Master::OnInventoryCrafterSlotSecondaryClicked(const int Slot, const bool bUpdateHUD)
{
    bool bContentsChanged = false;
    this->InventoryCrafter[Slot].OnSecondaryClicked(this, bContentsChanged);

    if (bContentsChanged == false)
    {
        return;
    }

    if (bUpdateHUD)
    {
        HUD->RefreshCurrentOpenedContainerWithCharacterHotbar();
    }
}

void ACH_Master::OnInventoryCrafterProductClicked(const bool bUpdateHUD)
{
    const FAccumulated Product = this->GetInventoryCrafterProduct();
    
    // We ofc can not craft something if we already have something in hand.
    /* TODO We later have to check if the item that we have in hand is the same as the output. */
    if (this->CursorHand != FAccumulated::NullAccumulated)
    {
        if (this->CursorHand == Product)
        {
            this->ReduceInventoryCrafterByProductIngredients(false);
            this->CursorHand.Amount += Product.Amount;

            if (bUpdateHUD)
            {
                HUD->RefreshCurrentOpenedContainerWithCharacterHotbar();
            }
            
            return;
        }
        
        return;
    }
    
    if (Product == FAccumulated::NullAccumulated)
    {
        return;
    }

    this->ReduceInventoryCrafterByProductIngredients(false);
    this->CursorHand = Product;

    if (bUpdateHUD)
    {
        HUD->RefreshCurrentOpenedContainerWithCharacterHotbar();
    }
    
    return;
}

FAccumulated ACH_Master::GetInventoryCrafterProduct(void) const
{
    if (this->InventoryCrafter.Num() < 1)
    {
        UIL_LOG(Warning, TEXT("ACH_Master::GetInventoryCrafterProduct: Tried to get the product of the inventory crafter, but it is not initialized yet."));
        return FAccumulated::NullAccumulated;
    }
    
    FAccumulated Product;
    GAME_MODE->PrescriptionSeeker->GetProduct(this->GetInventoryCrafterAsDelivery(), Product);

    return Product;
}

void ACH_Master::ClearInventoryCrafterSlots(const bool bUpdateHUD)
{
    for (int i = 0; i < this->InventoryCrafter.Num(); ++i)
    {
        if (this->InventoryCrafter[i].Content == FAccumulated::NullAccumulated)
        {
            continue;
        }

        const FAccumulated Temp             = this->InventoryCrafter[i].Content;
        this->InventoryCrafter[i].Content   = FAccumulated::NullAccumulated;

        if (this->AddToInventory(Temp, false))
        {
            continue;
        }

        /* TODO Drop the accumulated item. */
        
        continue;
    }

    if (bUpdateHUD)
    {
        HUD->RefreshCurrentOpenedContainerWithCharacterHotbar();
    }

    return;
}

#define SLOT_SAFE_ADD_POST_BEHAVIOR(State, Slot, SlotArray) if (State == false) { UIL_LOG(Fatal, TEXT("PLACEHOLDER: Failed to safe add the amount of accumualted item for %d slot. Found %s."), Slot, *SlotArray[Slot].Content.ToString()) return; } 


void ACH_Master::AddToInventoryAtSlot(const int Slot, const int Amount, const bool bUpdateHUD)
{
    bool bSuccess;
    this->Inventory[Slot].Content.SafeAddAmount(Amount, bSuccess);
    SLOT_SAFE_ADD_POST_BEHAVIOR(bSuccess, Slot, this->Inventory)
    
    if (bUpdateHUD)
    {
        HUD->RefreshCharacterHotbar();
        this->UpdateItemPreview();
    }

    return;
}

void ACH_Master::AddToInventoryCrafterAtSlot(const int Slot, const int Amount, const bool bUpdateHUD)
{
    // this->InventoryCrafter[Slot].Content.Amount += Amount;
    // if (this->InventoryCrafter[Slot].Content.Amount <= 0)
    // {
    //     this->InventoryCrafter[Slot].Content = FAccumulated::NullAccumulated;
    // }

    bool bSuccess;
    this->InventoryCrafter[Slot].Content.SafeAddAmount(Amount, bSuccess);


    SLOT_SAFE_ADD_POST_BEHAVIOR(bSuccess, Slot, this->InventoryCrafter)

    // if (bSuccess == false)
    // {
    //     UIL_LOG(Fatal, TEXT("ACH_Master::AddToInventoryCrafterAtSlot: Failed to safe add the amount of the accumulated item in the inventory crafter for %d slot. Found %s."),
    //         Slot, *this->InventoryCrafter[Slot].Content.ToString()
    //     );
    //     return;
    // }
    
    if (bUpdateHUD)
    {
        HUD->RefreshCurrentOpenedContainerWithCharacterHotbar();
    }

    return;
}

void ACH_Master::ReduceInventoryCrafterByProductIngredients(const bool bUpdateHUD)
{
    /* We can just count for every slot in the crafter one down, right? */

    for (int i = 0; i < this->InventoryCrafter.Num(); ++i)
    {
        if (this->InventoryCrafter[i].Content == FAccumulated::NullAccumulated)
        {
            continue;
        }

        bool bSuccess;
        this->InventoryCrafter[i].Content.SafeAddAmount(-1, bSuccess);
        if (bSuccess == false)
        {
            UIL_LOG(
                Fatal,
                TEXT("ACH_Master::ReduceInventoryCrafterByProductIngredients: Failed to safe reduce the amount of the accumulated item in the inventory crafter for %d slot. Found %s."),
                i, *this->InventoryCrafter[i].Content.ToString()
            );
            return;
        }

        continue;
    }
    
    if (bUpdateHUD)
    {
        HUD->RefreshCurrentOpenedContainerWithCharacterHotbar();
    }

    return;
}

#pragma endregion Inventory Manipulation

#pragma endregion Member Methods

#pragma region Getters

void ACH_Master::GetTargetedVoxel(AChunk*& OutChunk, FVector& OutWorldHitLocation, FVector_NetQuantizeNormal& OutWorldNormalHitLocation, FIntVector& OutLocalHitVoxelLocation, const float UnrealReach) const
{
    OutChunk                    = nullptr;
    OutLocalHitVoxelLocation    = FIntVector::ZeroValue;
    
    const FTransform        TraceStart  = this->GetFirstPersonTraceStart();
    const FVector           TraceEnd    = TraceStart.GetLocation() + TraceStart.GetRotation().Vector() * UnrealReach;
    FCollisionQueryParams   TraceParams = FCollisionQueryParams(FName(TEXT("")), false, this->GetOwner());

    FHitResult HitResult;
    this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart.GetLocation(), TraceEnd, ECollisionChannel::ECC_Visibility, TraceParams);

    if (HitResult.GetActor() == nullptr)
    {
        return;
    }

    if ((OutChunk = Cast<AChunk>(HitResult.GetActor())) != nullptr)
    {
        OutWorldHitLocation         = HitResult.Location - HitResult.Normal;
        OutWorldNormalHitLocation   = HitResult.Normal;
        OutLocalHitVoxelLocation    = AChunkWorld::WorldToLocalVoxelPosition(OutWorldHitLocation);
        return;
    }

    return;
}

#pragma endregion Getters

#undef UIL_LOG
#undef HUD
#undef ENHANCED_SUBSYSTEM
#undef PLAYER_CONTROLLER
#undef GAME_MODE
#undef GAME_INSTANCE
#undef ENTITY_MASTER
#undef SLOT_SAFE_ADD_POST_BEHAVIOR
