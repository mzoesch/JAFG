// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Character/MyCharacterMovementComponent.h"

UMyCharacterMovementComponent::UMyCharacterMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->GravityScale               = 2.0f;
    this->JumpZVelocity              = 700.0f;
    this->AirControl                 = 2.0f;

    /* Flying */
    this->BrakingDecelerationFlying  = 2048.0f;

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
