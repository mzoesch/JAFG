// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputTriggers.h"
#include "Camera/CameraComponent.h"
#include "WorldCore/Character/MyCharacterMovementComponent.h"

#include "WorldCharacter.generated.h"

class ACommonChunk;
class USpringArmComponent;
class UInputComponent;
struct FInputActionValue;

DECLARE_MULTICAST_DELEGATE(FOnCamerChangedEventSignature);

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
    void ListenForCameraChangedEventWithNonFPMeshWrapper(void);

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


#pragma endregion Member Methods

#pragma region Member Variables

    float DefaultFieldOfView = 120.0f;
    float ZoomedFieldOfView  = 60.0f;
    float DefaultOrthoWidth  = 8192.0f;

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
    FORCEINLINE auto GetMyCharacterMovement(void) const -> UMyCharacterMovementComponent*
    {
        return this->GetCharacterMovement<UMyCharacterMovementComponent>();
    }

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    /** Override this method to add custom key bindings in derived classes. */
    virtual auto BindAction(const FString& ActionName, UEnhancedInputComponent* EnhancedInputComponent) -> void;

    virtual void OnTriggeredMove(const FInputActionValue& Value);
    virtual void OnTriggeredLook(const FInputActionValue& Value);
    virtual void OnStartedJump(const FInputActionValue& Value);
    virtual void OnTriggerJump(const FInputActionValue& Value);
    virtual void OnCompleteJump(const FInputActionValue& Value);
    virtual void OnTriggerCrouch(const FInputActionValue& Value);
    virtual void OnCompleteCrouch(const FInputActionValue& Value);
    virtual void OnStartedPrimary(const FInputActionValue& Value);
    UFUNCTION(Server, Reliable, WithValidation)
    virtual void OnStartedPrimary_ServerRPC(const FInputActionValue& Value);
    virtual void OnStartedSecondary(const FInputActionValue& Value);
    virtual void OnTriggeredUpMaxFlySpeed(const FInputActionValue& Value);
    virtual void OnTriggeredDownMaxFlySpeed(const FInputActionValue& Value);

    virtual void OnToggleCameras(const FInputActionValue& Value);
    virtual void OnTriggerZoomCameras(const FInputActionValue& Value);
    virtual void OnCompleteZoomCameras(const FInputActionValue& Value);
    virtual void OnTogglePerspective(const FInputActionValue& Value);

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

    auto ToggleFly(void) const -> void;
    auto ToggleInputFly(void) const -> void;

#pragma endregion Command Interface

#pragma region World Interaction

    FORCEINLINE auto GetFPSCamera(void) const -> UCameraComponent* { return this->FirstPersonCameraComponent; }

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
