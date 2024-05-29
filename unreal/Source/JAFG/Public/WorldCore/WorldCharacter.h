// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/Character.h"
#include "InputTriggers.h"
#include "Camera/CameraComponent.h"
#include "Player/WorldPlayerController.h"
#include "WorldCore/Character/MyCharacterMovementComponent.h"

#include "WorldCharacter.generated.h"

JAFG_VOID

class UJAFGInputSubsystem;
class UInputMappingContext;
class ACommonChunk;
class USpringArmComponent;
class UInputComponent;
struct FInputActionValue;

DECLARE_MULTICAST_DELEGATE(FOnCamerChangedEventSignature)

UCLASS(Abstract)
class JAFG_API AWorldCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    explicit AWorldCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // AActor implementation
    virtual void BeginPlay(void) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    // ~AActor implementation

    //////////////////////////////////////////////////////////////////////////
    // AActor Components
    //////////////////////////////////////////////////////////////////////////

    /**
     * A wrapper around primitive components that only should be visible for other players or the owning player
     * if he is in any third-person camera mode.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UPrimitiveComponent> NonFPMeshWrapper;
    auto ListenForCameraChangedEventWithNonFPMeshWrapper(void) -> void;
    auto UpdateFOVBasedOnSprintState(void) const -> void;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> ThirdPersonSpringArmComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> ThirdPersonCameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> ThirdPersonFrontSpringArmComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> ThirdPersonFrontCameraComponent;

#pragma region Member Methods

    FORCEINLINE AWorldPlayerController* GetWorldPlayerController(void) const { return this->GetController<AWorldPlayerController>(); }

#pragma endregion Member Methods

#pragma region Member Variables

    float DefaultFieldOfView          = 120.0f;
    float SprintFieldOfViewMultiplier = 1.1f;
    float ZoomedFieldOfView           = 60.0f;
    float DefaultOrthoWidth           = 8192.0f;

#pragma endregion Member Variables

#pragma region Enhanced Input

public:

    //////////////////////////////////////////////////////////////////////////
    // Enhanced Input Extern Events
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto IsFlying(void) const -> bool { return this->GetMyCharacterMovement()->IsFlying(); }
    FORCEINLINE auto IsInputFlyEnabled(void) const -> bool { return this->GetMyCharacterMovement()->bAllowInputFly; }

private:

    FOnCamerChangedEventSignature OnCameraChangedEvent;

protected:

    /** Shortcut to the JAFG custom movement component. */
    FORCEINLINE auto GetMyCharacterMovement(void) const -> UMyCharacterMovementComponent* { return this->GetCharacterMovement<UMyCharacterMovementComponent>(); }

    virtual auto SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) -> void override;

    virtual auto SetFootContextBasedOnCharacterState(const bool bClearOldMappings = true, const int32 Priority = 0) -> void;
    virtual auto GetFootContextBasedOnCharacterState(void) const -> UInputMappingContext*;
    virtual auto GetSafeFootContextBasedOnCharacterState(void) const -> UInputMappingContext*;
    virtual auto GetFootContextBasedOnCharacterState(UJAFGInputSubsystem* JAFGInputSubsystem) const -> UInputMappingContext*;
    virtual auto GetSafeFootContextBasedOnCharacterState(UJAFGInputSubsystem* JAFGInputSubsystem) const -> UInputMappingContext*;

    /** Override this method to add custom key bindings in derived classes. */
    virtual auto BindAction(const FString& ActionName, UEnhancedInputComponent* EnhancedInputComponent) -> void;

    virtual auto OnTriggeredMove(const FInputActionValue& Value) -> void;
    virtual auto OnTriggeredLook(const FInputActionValue& Value) -> void;
    virtual auto OnStartedJump(const FInputActionValue& Value) -> void;
    virtual auto OnTriggerJump(const FInputActionValue& Value) -> void;
    virtual auto OnCompleteJump(const FInputActionValue& Value) -> void;
    virtual auto OnStartedSprint(const FInputActionValue& Value) -> void;
    virtual auto OnCompletedSprint(const FInputActionValue& Value) -> void;
    virtual auto OnTriggerCrouch(const FInputActionValue& Value) -> void;
    virtual auto OnCompleteCrouch(const FInputActionValue& Value) -> void;
    virtual auto OnStartedPrimary(const FInputActionValue& Value) -> void;
    UFUNCTION(Server, Reliable)
    virtual void OnStartedPrimary_ServerRPC(const FInputActionValue& Value);
    virtual auto OnStartedSecondary(const FInputActionValue& Value) -> void;
    UFUNCTION(Server, Reliable)
    virtual void OnStartedSecondary_ServerRPC(const FInputActionValue& Value);
    virtual auto OnTriggeredUpMaxFlySpeed(const FInputActionValue& Value) -> void;
    virtual auto OnTriggeredDownMaxFlySpeed(const FInputActionValue& Value) -> void;

    virtual auto OnToggleCameras(const FInputActionValue& Value) -> void;
    virtual auto OnTriggerZoomCameras(const FInputActionValue& Value) -> void;
    virtual auto OnCompleteZoomCameras(const FInputActionValue& Value) -> void;
    bool bZooming = false;
    virtual auto OnTogglePerspective(const FInputActionValue& Value) -> void;

    float LastJumpStarted = 0.0f;
    static constexpr float JumpFlyModeDeactivationTime = 0.25f;

private:

    auto BindAction(
        const FString& ActionName,
        UEnhancedInputComponent* EnhancedInputComponent,
        const ETriggerEvent Event,
        void (AWorldCharacter::* Method) (const FInputActionValue& Value)
    ) -> void;

protected:

    //////////////////////////////////////////////////////////////////////////
    // Enhanced Event Bindings
    //////////////////////////////////////////////////////////////////////////

    FDelegateHandle EscapeMenuVisibilityChangedHandle;
    virtual auto OnEscapeMenuVisibilityChanged(const bool bVisible) -> void;

#pragma endregion Enhanced Input

#pragma region Command Interface

public:

    //////////////////////////////////////////////////////////////////////////
    // Command Interface
    //////////////////////////////////////////////////////////////////////////

    auto ToggleFly(void) -> void;
    auto ToggleInputFly(void) const -> void;

#pragma endregion Command Interface

#pragma region World Interaction

    FORCEINLINE auto GetFPSCamera(void) const -> UCameraComponent* { return this->FirstPersonCameraComponent; }
    FORCEINLINE auto GetDisplayName(void) const -> FString { return this->GetWorldPlayerController()->GetDisplayName(); }

    //////////////////////////////////////////////////////////////////////////
    // World Locations And Rotations
    //////////////////////////////////////////////////////////////////////////

    /**
     * The maximum distance where we should ever trace for voxels and other hit objects.
     * Equivalent to: four chunks => 4 x 16 Voxels => 64 Voxels.
     */
    inline static constexpr float MaxPOVLineTraceLength { ( WorldStatics::ChunkSize * 4.0f ) * WorldStatics::JToUScale };
    /* Packed into a method for future easier change per-player basis.  */
    // ReSharper disable once CppMemberFunctionMayBeStatic
    FORCEINLINE auto GetCharacterReachInVoxels(void) const -> float { return 4.5f; }
    /** In unreal scale. */
    FORCEINLINE auto GetCharacterReach(void) const -> float { return this->GetCharacterReachInVoxels() * WorldStatics::JToUScale; }

    /**
     * APawn#RemoteViewPitch is an uint8 and represents the pitch of the remote view replicated back to the server.
     * This method converts the pitch to degrees.
     * It does not work on listens servers for local controlled characters.
     */
    FORCEINLINE auto GetRemoteViewPitchAsDeg(void) const -> float { return this->RemoteViewPitch * 360.0f / /* 2^8 - 1 = */ 255.0f; }
    FORCEINLINE auto GetFirstPersonTraceStart(void) const -> FTransform { return this->FirstPersonCameraComponent->GetComponentTransform(); }
    /** Will return falsely values on local controlled characters. */
    FORCEINLINE auto GetNonLocalFirstPersonTraceStart(void) const -> FTransform
    {
        return FTransform(
            FQuat(FRotator(
                this->GetRemoteViewPitchAsDeg(),
                this->GetFirstPersonTraceStart().Rotator().Yaw,
                this->GetFirstPersonTraceStart().Rotator().Roll
            )),
            this->GetFirstPersonTraceStart().GetLocation(),
            FVector::OneVector
        );
    }

    /** The added offset to the First-Person Camera Component. */
    inline static const FVector TorsoOffset { 0.0f, 0.0f, -50.0f };

    FORCEINLINE auto GetTorsoLocation(void) const -> FVector { return this->FirstPersonCameraComponent->GetComponentLocation() + AWorldCharacter::TorsoOffset; }
    FORCEINLINE auto GetTorsoRelativeLocation(void) const -> FVector { return this->FirstPersonCameraComponent->GetRelativeLocation() + AWorldCharacter::TorsoOffset; }
    FORCEINLINE auto GetTorsoTransform(void) const -> FTransform { return FTransform(this->FirstPersonCameraComponent->GetComponentRotation(), this->GetTorsoLocation()); }

    FORCEINLINE auto GetFeetLocation(void) const -> FVector { return this->GetActorLocation(); }

    void GetPOVTargetedData(
        ACommonChunk*& OutChunk,
        FVector& OutWorldHitLocation,
        FVector_NetQuantizeNormal& OutWorldNormalHitLocation,
        FVoxelKey& OutLocalHitVoxelKey,
        const bool bUseRemotePitch,
        const float UnrealReach
    ) const;

#pragma endregion World Interaction

};
