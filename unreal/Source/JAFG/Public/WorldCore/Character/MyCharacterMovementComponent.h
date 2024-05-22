// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MyCharacterMovementComponent.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API UMyCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:

    explicit UMyCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#pragma region Member Variables

    float MaxAccelerationWalking         = 2048.0f;

    float AbsoluteMaxFlySpeed            = 16384.0f;
    float AbsoluteMinFlySpeed            = 100.0f;
    float FlySpeedSteps                  = 100.0f;
    float MaxAccelerationFly             = 16384.0f;
    bool  bAllowInputFly                 = false;

#pragma endregion Member Variables

    void IncrementFlySpeed(void);
    void DecrementFlySpeed(void);

protected:

    UFUNCTION(Server, Reliable)
    void IncrementFlySpeed_ServerRPC();
    UFUNCTION(Server, Reliable)
    void DecrementFlySpeed_ServerRPC();

    virtual void OnMovementModeChanged(const EMovementMode PreviousMovementMode, const uint8 PreviousCustomMode) override;
};
