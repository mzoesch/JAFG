// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Character/MyCharacterMovementComponent.h"

UMyCharacterMovementComponent::UMyCharacterMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->GravityScale               = 3.0f;
    this->JumpZVelocity              = 1200.0f;
    this->AirControl                 = 2.0f;
    this->MaxAcceleration            = this->MaxAccelerationWalking;

    /* Flying */
    this->BrakingDecelerationFlying  = 8192.0f;
    this->MaxFlySpeed                = 600.0f;

    /* Stepping */
    this->MaxStepHeight              = 60.0f;
    this->bUseFlatBaseForFloorChecks = true;
    this->LedgeCheckThreshold        = 4.0f;

    /* Crouching */
    this->NavAgentProps.bCanCrouch       = true;
    this->bCanWalkOffLedgesWhenCrouching = false;
    this->SetCrouchedHalfHeight(75.0f);
    this->MaxWalkSpeedCrouched           = this->MaxWalkSpeed * 0.25f;

    return;
}

void UMyCharacterMovementComponent::IncrementFlySpeed(void)
{
    const float OldSpeed = this->MaxFlySpeed;
    this->MaxFlySpeed = FMath::Clamp(this->MaxFlySpeed + this->FlySpeedSteps, this->AbsoluteMinFlySpeed, this->AbsoluteMaxFlySpeed);

    if (OldSpeed != this->MaxFlySpeed && UNetStatics::IsSafeClient(this))
    {
        this->IncrementFlySpeed_ServerRPC();
    }

#if WITH_EDITOR
    if (this->MaxFlySpeed == this->AbsoluteMaxFlySpeed)
    {
        LOG_VERBOSE(LogWorldChar, "Reached max fly speed.")
    }
#endif /* WITH_EDITOR */

    return;
}

void UMyCharacterMovementComponent::DecrementFlySpeed(void)
{
    const float OldSpeed = this->MaxFlySpeed;
    this->MaxFlySpeed = FMath::Clamp(this->MaxFlySpeed - this->FlySpeedSteps, this->AbsoluteMinFlySpeed, this->AbsoluteMaxFlySpeed);

    if (OldSpeed != this->MaxFlySpeed && UNetStatics::IsSafeClient(this))
    {
        this->DecrementFlySpeed_ServerRPC();
    }

#if WITH_EDITOR
    if (this->MaxFlySpeed == this->AbsoluteMinFlySpeed)
    {
        LOG_VERBOSE(LogWorldChar, "Reached min fly speed.")
    }
#endif /* WITH_EDITOR */

    return;
}

void UMyCharacterMovementComponent::IncrementFlySpeed_ServerRPC_Implementation()
{
    this->MaxFlySpeed = FMath::Clamp(this->MaxFlySpeed + this->FlySpeedSteps, this->AbsoluteMinFlySpeed, this->AbsoluteMaxFlySpeed);
}

void UMyCharacterMovementComponent::DecrementFlySpeed_ServerRPC_Implementation()
{
    this->MaxFlySpeed = FMath::Clamp(this->MaxFlySpeed - this->FlySpeedSteps, this->AbsoluteMinFlySpeed, this->AbsoluteMaxFlySpeed);
}

void UMyCharacterMovementComponent::OnMovementModeChanged(const EMovementMode PreviousMovementMode, const uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

    if (this->MovementMode == EMovementMode::MOVE_Flying)
    {
        this->MaxAcceleration = this->MaxAccelerationFly;
    }
    else
    {
        this->MaxAcceleration = this->MaxAccelerationWalking;
    }

    return;
}
