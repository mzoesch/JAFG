// © 2023 mzoesch. All rights reserved.

#include "Core/CH_Master.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"

#include "World/Chunk.h"
#include "World/ChunkWorld.h"
#include "HUD/HUD_Master.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Lib/Cuboid.h"

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

    this->GetCharacterMovement()->GravityScale = 2.0f;
    this->GetCharacterMovement()->JumpZVelocity = 700.0f;
    this->GetCharacterMovement()->AirControl = 2.0f;
    this->GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
    
    this->QuickSlotSelected = 0;
    this->Inventory.SetNum(10, false);
    this->Inventory[0] = FAccumulated(EVoxel::Stone);
    this->Inventory[1] = FAccumulated(EVoxel::Dirt);
    this->Inventory[2] = FAccumulated(EVoxel::Grass);
    
    return;
}

void ACH_Master::BeginPlay()
{
    Super::BeginPlay();

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

    this->ItemPreview->GenerateMesh(this->GetQuickSlotSelected());
    
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

    APlayerController* PC = this->GetWorld()->GetFirstPlayerController();
    check(PC)

#if WITH_EDITOR
    if (!Cast<UEnhancedPlayerInput>(PC->PlayerInput))
    {
        PC->ClientMessage(TEXT("Player controller is not using the Enhanced Input system."));
        UE_LOG(LogTemp, Error, TEXT("Player controller is not using the Enhanced Input system."))
        return;
    }
#endif /* WITH_EDITOR */
    
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
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
    this->GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart.GetLocation(),
        TraceEnd,
        ECC_Visibility,
        TraceParams
    )
    ;

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
    Chunk->ModifyVoxel(LocalVoxelPos, EVoxel::Air);
	
    return;
}

void ACH_Master::OnSecondary(const FInputActionValue& Value)
{
    /* TODO Here than check for tools and call the method. */

    const FTransform TraceStart = FirstPersonCameraComponent->GetComponentTransform();
    const FVector TraceEnd = TraceStart.GetLocation() + (TraceStart.GetRotation().Vector() * (4 * 100));
    const FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("")), false, this->GetOwner());

    FHitResult HitResult;
    this->GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart.GetLocation(),
        TraceEnd,
        ECC_Visibility,
        TraceParams
    )
    ;

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
    Chunk->ModifyVoxel(BlockPosTargetInt, static_cast<EVoxel>(this->QuickSlotSelected + 2));

    return;
}

void ACH_Master::OnQuickSlot0(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(9);
    return;
}

void ACH_Master::OnQuickSlot1(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(0);
    return;
}

void ACH_Master::OnQuickSlot2(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(1);
    return;
}

void ACH_Master::OnQuickSlot3(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(2);
    return;
}

void ACH_Master::OnQuickSlot4(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(3);
    return;
}

void ACH_Master::OnQuickSlot5(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(4);
    return;
}

void ACH_Master::OnQuickSlot6(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(5);
    return;
}

void ACH_Master::OnQuickSlot7(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(6);
    return;
}

void ACH_Master::OnQuickSlot8(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(7);
    return;
}

void ACH_Master::OnQuickSlot9(const FInputActionValue& Value)
{
    this->OnQuickSlotSelect(8);
    return;
}

void ACH_Master::OnQuickSlotSelect(const int Slot)
{
    this->QuickSlotSelected = Slot;
    CastChecked<AHUD_Master>(this->GetWorld()->GetFirstPlayerController()->GetHUD())->OnQuickSlotSelect();
    this->ItemPreview->GenerateMesh(this->GetQuickSlotSelected());
    return;
}

#pragma endregion Input Actions
