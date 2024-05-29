// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MyCharacterMovementComponent.generated.h"

JAFG_VOID

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSprintStateChangedSignature, const bool /* bSprinting */)

UCLASS(NotBlueprintable)
class JAFG_API UMyCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:

    explicit UMyCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#pragma region Member Variables

    float MaxAccelerationWalking         = 2048.0f;

    /*
     * Flying
     */

    float FlyBreakDeceleration           = 32768.0f;
    float AbsoluteMaxFlySpeed            = 16384.0f;
    float AbsoluteMinFlySpeed            = 100.0f;
    float FlySpeedSteps                  = 400.0f;
    float MaxAccelerationFly             = FLT_MAX;
    bool  bAllowInputFly                 = false;

    /*
     * Stepping
     */
    float NormalMaxWalkingSpeed          = 600.0f;

    float SprintMaxWalkSpeedMultiplier   = 2.0f;

#pragma endregion Member Variables

    void IncrementFlySpeed(void);
    void DecrementFlySpeed(void);

    virtual void Crouch(const bool bClientSimulation) override;
    virtual void UnCrouch(const bool bClientSimulation) override;

    void SetWantsToSprint(const bool bInWantsToSprint);
    FORCEINLINE auto IsWantingToSprint(void) const -> bool { return this->bWantsToSprint; }
    FORCEINLINE auto IsSprinting(void) const -> bool { return this->bSpringing; }

    /** Delegate only broadcasted on the local owner. */
    FOnSprintStateChangedSignature OnSprintStateChangedDelegate;

protected:

    UFUNCTION(Server, Reliable)
    void IncrementFlySpeed_ServerRPC();
    UFUNCTION(Server, Reliable)
    void DecrementFlySpeed_ServerRPC();

    virtual void OnMovementModeChanged(const EMovementMode PreviousMovementMode, const uint8 PreviousCustomMode) override;

    /** Will check the current state of the component and then sprint or not. */
    void EvaluateSprint(void);
    UFUNCTION(Server, Reliable)
    void EvaluateSprint_ServerRPC(const bool bRemoteWantsToSprint);

private:

    /**
     * If the user wants to sprint (pressing the keys while in the correct context).
     * Therefore, it only exists on the local owner. Meaningless on a remote component.
     */
    bool bWantsToSprint = false;
    bool bSpringing     = false;
};
