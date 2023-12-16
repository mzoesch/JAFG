// © 2023 mzoesch. All rights reserved.

#include "Core/CH_Master.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"

#include "World/Chunk.h"
#include "World/ChunkWorld.h"

ACH_Master::ACH_Master()
{
    // Set this character to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    this->GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);

    this->FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    this->FirstPersonCameraComponent->SetupAttachment(this->GetCapsuleComponent());
    this->FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f));
    this->FirstPersonCameraComponent->bUsePawnControlRotation = true;
    this->FirstPersonCameraComponent->SetFieldOfView(120.0f);
    
    return;
}

void ACH_Master::BeginPlay()
{
    Super::BeginPlay();
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
    Chunk->ModifyVoxel(BlockPosTargetInt, EVoxel::Stone);

    return;
}

#pragma endregion Input Actions
