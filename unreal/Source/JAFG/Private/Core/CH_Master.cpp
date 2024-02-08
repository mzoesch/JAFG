// © 2023 mzoesch. All rights reserved.

#include "Core/CH_Master.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Core/GM_Master.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Entity/Drop.h"
#include "World/Chunk.h"
#include "World/ChunkWorld.h"
#include "HUD/HUD_Master.h"
#include "Lib/Cuboid.h"
#include "Core/PC_Master.h"
#include "..\..\Public\Lib\PrescriptionSeeker.h"
#include "World/WorldVoxel.h"

#define UIL_LOG(Verbosity, Format, ...)     UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define HUD                                 Cast<AHUD_Master>(this->GetWorld()->GetFirstPlayerController()->GetHUD())
#define ENHANCED_SUBSYSTEM                  ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Cast<APlayerController>(this->GetWorld()->GetFirstPlayerController())->GetLocalPlayer())
#define MASTER_PC                           Cast<APC_Master>(this->GetWorld()->GetFirstPlayerController())

ACH_Master::ACH_Master()
{
    this->PrimaryActorTick.bCanEverTick = false;

    this->GetCapsuleComponent()->InitCapsuleSize(40.0f, 90.0f);

    this->FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    this->FirstPersonCameraComponent->SetupAttachment(this->GetCapsuleComponent());
    this->FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f));
    this->FirstPersonCameraComponent->bUsePawnControlRotation = true;
    this->FirstPersonCameraComponent->SetFieldOfView(120.0f);

    this->ItemPreview = nullptr;

    this->GetCharacterMovement()->GravityScale                  = 2.0f;
    this->GetCharacterMovement()->JumpZVelocity                 = 700.0f;
    this->GetCharacterMovement()->AirControl                    = 2.0f;
    this->GetCharacterMovement()->bUseFlatBaseForFloorChecks    = true;
    
    this->SelectedQuickSlot = 0;
    this->Inventory.SetNum(ACH_Master::InventoryStartSize, false);
    
    return;
}

void ACH_Master::BeginPlay()
{
    Super::BeginPlay();

    this->Inventory.SetNum(ACH_Master::InventoryStartSize, false);
    this->InventoryCrafter.SetNum(ACH_Master::InventoryCrafterSize, false);

    this->ItemPreview = this->GetWorld()->SpawnActor<ACuboid>(
        ACuboid::StaticClass(),
        FTransform(
            FRotator(190.0f, 100.0f, -10.0f),
            FVector(58.0f, 65.0f, -38.0f),
            FVector(0.12f, 0.12f, 0.12f)
        ),
        FActorSpawnParameters()
    );
    this->ItemPreview->AttachToComponent(this->FirstPersonCameraComponent, FAttachmentTransformRules::KeepRelativeTransform);
    this->ItemPreview->GenerateMesh(this->GetAccumulatedAtSelectedQuickSlot());

    this->AccumulatedInCursorHand = FAccumulated::NullAccumulated;
    
    return;
}

void ACH_Master::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
    return;
}

void ACH_Master::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    check(MASTER_PC)

#if WITH_EDITOR
    if (!Cast<UEnhancedPlayerInput>(MASTER_PC->PlayerInput))
    {
        MASTER_PC->ClientMessage(TEXT("Player controller is not using the Enhanced Input system."));
        UE_LOG(LogTemp, Error, TEXT("Player controller is not using the Enhanced Input system."))
        return;
    }
#endif /* WITH_EDITOR */
    
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_SUBSYSTEM)
    {
        check(Subsystem)
        
        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(this->IMCFoot, 0);
        UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
        
        /* Movement */
        EIC->BindAction(this->IAMove, ETriggerEvent::Triggered, this, &ACH_Master::OnMove);
        EIC->BindAction(this->IALook, ETriggerEvent::Triggered, this, &ACH_Master::OnLook);
        EIC->BindAction(this->IAJump, ETriggerEvent::Triggered, this, &ACH_Master::OnTriggerJump);
        EIC->BindAction(this->IAJump, ETriggerEvent::Completed, this, &ACH_Master::OnCompleteJump);

        /* Interaction */
        /* TODO This has ofc to be Triggered. */
        EIC->BindAction(this->IAPrimary, ETriggerEvent::Started, this, &ACH_Master::OnPrimary);
        EIC->BindAction(this->IASecondary, ETriggerEvent::Started, this, &ACH_Master::OnSecondary);

        /* Inventory */
        EIC->BindAction(this->IAToggleInventory, ETriggerEvent::Started, this, &ACH_Master::OnInventoryToggle);
        EIC->BindAction(this->IAQuickSlot0, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot0);
        EIC->BindAction(this->IAQuickSlot1, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot1);
        EIC->BindAction(this->IAQuickSlot2, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot2);
        EIC->BindAction(this->IAQuickSlot3, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot3);
        EIC->BindAction(this->IAQuickSlot4, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot4);
        EIC->BindAction(this->IAQuickSlot5, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot5);
        EIC->BindAction(this->IAQuickSlot6, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot6);
        EIC->BindAction(this->IAQuickSlot7, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot7);
        EIC->BindAction(this->IAQuickSlot8, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot8);
        EIC->BindAction(this->IAQuickSlot9, ETriggerEvent::Started, this, &ACH_Master::OnQuickSlot9);

        /* MISC */
        EIC->BindAction(this->IADebugScreen, ETriggerEvent::Started, this, &ACH_Master::OnDebugScreenToggle);
    }

    return;
}

#pragma region Input Actions

void ACH_Master::OnMove(const FInputActionValue& Value)
{
    this->AddMovementInput(this->GetActorForwardVector(), Value.Get<FVector2D>().Y);
    this->AddMovementInput(this->GetActorRightVector(), Value.Get<FVector2D>().X);
    return;
}

void ACH_Master::OnLook(const FInputActionValue& Value)
{
    this->AddControllerYawInput(Value.Get<FVector2D>().X * 0.2f);
    this->AddControllerPitchInput(Value.Get<FVector2D>().Y * -0.2f);
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
    /* TODO Here than check for tools and call the method. */

    const FTransform TraceStart = FirstPersonCameraComponent->GetComponentTransform();
    const FVector TraceEnd = TraceStart.GetLocation() + (TraceStart.GetRotation().Vector() * (4 * 100));
    const FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("")), false, this->GetOwner());

    FHitResult HitResult;
    this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart.GetLocation(), TraceEnd, ECC_Visibility, TraceParams);

    if (HitResult.GetActor() == nullptr)
    {
        return;
    }

    if (HitResult.GetActor()->IsA(AChunk::StaticClass()) == false)
    {
        return;
    }
	
    AChunk* Chunk = CastChecked<AChunk>(HitResult.GetActor());
    const FVector RealPos = HitResult.Location - HitResult.Normal;
    FIntVector LocalVoxelPos = AChunkWorld::WorldToLocalVoxelPosition(RealPos);
    int V = Chunk->GetVoxel(LocalVoxelPos);
    if (V == EWorldVoxel::VoxelNull || V == EWorldVoxel::AirVoxel)
    {
        return;
    }
    
    Chunk->ModifyVoxel(LocalVoxelPos, EWorldVoxel::AirVoxel);

    /* Drop the voxel. */
    FVector DropPos = HitResult.Location + HitResult.Normal;
    DropPos.Z += 1.0f;
    ADrop* Drop = this->GetWorld()->SpawnActorDeferred<ADrop>(ADrop::StaticClass(), FTransform(FRotator::ZeroRotator, DropPos, FVector::OneVector));
    Drop->SetVoxel(V);
    UGameplayStatics::FinishSpawningActor(Drop, FTransform(FRotator(0.0f, 0.0f, 180.0f), DropPos, FVector::OneVector));
    
    return;
}

void ACH_Master::OnSecondary(const FInputActionValue& Value)
{
    const FTransform TraceStart = FirstPersonCameraComponent->GetComponentTransform();
    const FVector TraceEnd = TraceStart.GetLocation() + (TraceStart.GetRotation().Vector() * (4 * 100));
    const FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("")), false, this->GetOwner());

    FHitResult HitResult;
    this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart.GetLocation(), TraceEnd, ECC_Visibility, TraceParams);

    if (HitResult.GetActor() == nullptr)
    {
        return;
    }

    if (!HitResult.GetActor()->IsA(AChunk::StaticClass()))
    {
        return;
    }
	
    AChunk* Chunk = CastChecked<AChunk>(HitResult.GetActor());
    const FVector RealPos = HitResult.Location - HitResult.Normal;
    FIntVector LocalVoxelPos = AChunkWorld::WorldToLocalVoxelPosition(RealPos);
    FVector BlockPosTarget = HitResult.Normal + FVector(LocalVoxelPos);
    FIntVector BlockPosTargetInt = FIntVector(BlockPosTarget);

    /* TODO Here than check for tools and call the method. Interaction with Target voxel? */

    if (this->Inventory[this->SelectedQuickSlot].GetVoxel() != EWorldVoxel::VoxelNull && this->Inventory[this->SelectedQuickSlot].GetVoxel() != EWorldVoxel::AirVoxel)
    {
        Chunk->ModifyVoxel(BlockPosTargetInt, this->Inventory[this->SelectedQuickSlot].GetVoxel());
        this->AddToInventoryAtSlot(this->SelectedQuickSlot, -1, true);
        return;
    }

    return;
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void ACH_Master::OnInventoryToggle(const FInputActionValue& Value)
{
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ENHANCED_SUBSYSTEM)
    {
        if (Subsystem->HasMappingContext(this->IMCFoot))
        {
            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(this->IMCInventory, 0);

            HUD->OnInventoryToggle(true);
            MASTER_PC->ShowMouseCursor(true, true);

            return;
        }

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(this->IMCFoot, 0);

        this->ExecuteBehaviorToClearAccumulatedInCursorHand(false);
        this->ExecuteBehaviorToClearInventoryCrafterSlots(false);
        
        HUD->OnInventoryToggle(false);
        MASTER_PC->ShowMouseCursor(false, false);

        return;
    }

    UIL_LOG(Error, TEXT("ACH_Master::OnToggleInventory: Enhanced Subsystem not found!"));
    
    return;
}

void ACH_Master::OnQuickSlot0(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(0);
    return;
}

void ACH_Master::OnQuickSlot1(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(1);
    return;
}

void ACH_Master::OnQuickSlot2(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(2);
    return;
}

void ACH_Master::OnQuickSlot3(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(3);
    return;
}

void ACH_Master::OnQuickSlot4(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(4);
    return;
}

void ACH_Master::OnQuickSlot5(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(5);
    return;
}

void ACH_Master::OnQuickSlot6(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(6);
    return;
}

void ACH_Master::OnQuickSlot7(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(7);
    return;
}

void ACH_Master::OnQuickSlot8(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(8);
    return;
}

void ACH_Master::OnQuickSlot9(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(9);
    return;
}

void ACH_Master::OnQuickSlotSelect(const int Slot)
{
    this->SelectedQuickSlot = Slot;
    HUD->OnQuickSlotSelect();
    this->ItemPreview->GenerateMesh(this->GetAccumulatedAtSelectedQuickSlot());
    return;
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void ACH_Master::OnDebugScreenToggle(const FInputActionValue& Value)
{
    HUD->OnDebugScreenToggle();
    return;
}

#pragma endregion Input Actions

#pragma region Member Methods

#pragma region Inventory Manipulation

void ACH_Master::ExecuteBehaviorToClearAccumulatedInCursorHand(const bool bUpdateHUD)
{
    if (this->AccumulatedInCursorHand == FAccumulated::NullAccumulated)
    {
        return;
    }

    const FAccumulated Temp         = this->AccumulatedInCursorHand;
    this->AccumulatedInCursorHand   = FAccumulated::NullAccumulated;
    if (this->AddToInventory(Temp, bUpdateHUD))
    {
        return;
    }

    /* TODO Drop the accumulated item. */

    return;    
}

bool ACH_Master::AddToInventory(const FAccumulated Accumulated, const bool bUpdateHUD)
{
    /* Check if there is an existing accumulated item already in the characters inventory. */
    for (int i = 0; i < this->Inventory.Num(); i++)
    {
        if (this->Inventory[i].GetVoxel() == Accumulated.GetVoxel())
        {
            this->AddToInventoryAtSlot(i, Accumulated.GetAmount(), bUpdateHUD);
            return true;
        }

        continue;
    }

    /* Add the new accumulated item to an empty inventory slot. */
    for (int i = 0; i < this->Inventory.Num(); i++)
    {
        if (this->Inventory[i] == FAccumulated::NullAccumulated)
        {
            this->OverrideInventoryAtSlot(i, Accumulated, bUpdateHUD);
            return true;
        }

        continue;
    }
    
    return false;
}

void ACH_Master::OnInventorySlotClicked(const int Slot, const bool bUpdateHUD)
{
    if (this->AccumulatedInCursorHand == FAccumulated::NullAccumulated)
    {
        if (this->Inventory[Slot] == FAccumulated::NullAccumulated)
        {
            return;
        }

        this->AccumulatedInCursorHand = this->Inventory[Slot];
        this->Inventory[Slot] = FAccumulated::NullAccumulated;

        if (bUpdateHUD)
        {
            HUD->OnInventorySlotSelect();
        }
        
        return;
    }

    this->Inventory[Slot]           = this->AccumulatedInCursorHand;
    this->AccumulatedInCursorHand   = FAccumulated::NullAccumulated;

    if (bUpdateHUD)
    {
        HUD->OnInventorySlotSelect();
    }

    return;
}

void ACH_Master::OnInventoryCrafterSlotClicked(const int Slot, const bool bUpdateHUD)
{
    if (this->AccumulatedInCursorHand == FAccumulated::NullAccumulated)
    {
        if (this->InventoryCrafter[Slot] == FAccumulated::NullAccumulated)
        {
            return;
        }

        this->AccumulatedInCursorHand = this->InventoryCrafter[Slot];
        this->InventoryCrafter[Slot] = FAccumulated::NullAccumulated;

        if (bUpdateHUD)
        {
            HUD->OnInventorySlotSelect();
        }

        return;
    }

    this->InventoryCrafter[Slot] = this->AccumulatedInCursorHand;
    this->AccumulatedInCursorHand = FAccumulated::NullAccumulated;

    if (bUpdateHUD)
    {
        HUD->OnInventorySlotSelect();
    }

    return;
}

void ACH_Master::ExecuteBehaviorToClearInventoryCrafterSlots(const bool bUpdateHUD)
{
    for (int i = 0; i < this->InventoryCrafter.Num(); ++i)
    {
        if (this->InventoryCrafter[i] == FAccumulated::NullAccumulated)
        {
            continue;
        }

        const FAccumulated Temp = this->InventoryCrafter[i];
        this->InventoryCrafter[i] = FAccumulated::NullAccumulated;

        if (this->AddToInventory(Temp, false))
        {
            continue;
        }

        /* TODO Drop the accumulated item. */
        
        continue;
    }

    if (bUpdateHUD)
    {
        HUD->UpdateInventoryAndHotbar();
    }

    return;
}

FAccumulated ACH_Master::GetInventoryCrafterProduct() const
{
    if (this->InventoryCrafter.Num() < 1)
    {
        UIL_LOG(Warning, TEXT("ACH_Master::GetInventoryCrafterProduct: Tried to get the product of the inventory crafter, but it is not initialized yet."));
        return FAccumulated::NullAccumulated;
    }
    
    const AGM_Master* G = CastChecked<AGM_Master>(UGameplayStatics::GetGameMode(this->GetWorld()));

    FAccumulated    Product;
    FDelivery       Delivery;
    Delivery.DeliveryWidth  = 2;
    Delivery.Delivery       = this->InventoryCrafter;

    G->PrescriptionSeeker->GetProduct(Delivery, Product);
    
    return Product;
}

void ACH_Master::OnInventoryCrafterDeliveryClicked(const bool bUpdateHUD)
{
    // We ofc can not craft something if we already have something in hand.
    /* TODO We later have to check if the item that we have in hand is the same as the output. */
    if (this->AccumulatedInCursorHand != FAccumulated::NullAccumulated)
    {
        return;
    }

    const FAccumulated Product = this->GetInventoryCrafterProduct();
    
    if (Product == FAccumulated::NullAccumulated)
    {
        return;
    }

    this->ReduceInventoryCrafterByProductIngredients(false);
    this->AccumulatedInCursorHand = Product;

    if (bUpdateHUD)
    {
        HUD->UpdateInventoryAndHotbar();
    }
    
    return;
}

void ACH_Master::AddToInventoryAtSlot(const int Slot, const int Amount, const bool bUpdateHUD)
{
    this->Inventory[Slot].AddAmount(Amount);
    if (this->Inventory[Slot].GetAmount() <= 0)
    {
        this->Inventory[Slot] = FAccumulated::NullAccumulated;
    }

    if (bUpdateHUD)
    {
        HUD->OnHotbarUpdate();
        this->ItemPreview->GenerateMesh(this->GetAccumulatedAtSelectedQuickSlot());
    }

    return;
}

void ACH_Master::OverrideInventoryAtSlot(const int Slot, const FAccumulated Accumulated, const bool bUpdateHUD)
{
    this->Inventory[Slot] = Accumulated;

    if (bUpdateHUD)
    {
        HUD->OnHotbarUpdate();
        this->ItemPreview->GenerateMesh(this->GetAccumulatedAtSelectedQuickSlot());
    }
    
    return;
}

void ACH_Master::ReduceInventoryCrafterByProductIngredients(const bool bUpdateHUD)
{
    /* TODO Reduce the inventory crafter by the product. */

    if (bUpdateHUD)
    {
        HUD->UpdateInventoryAndHotbar();
    }

    return;
}

#pragma endregion Inventory Manipulation

#pragma endregion Member Methods

#undef UIL_LOG
#undef HUD
#undef ENHANCED_SUBSYSTEM
#undef MASTER_PC
