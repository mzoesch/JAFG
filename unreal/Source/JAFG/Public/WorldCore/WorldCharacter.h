// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputTriggers.h"
#include "Camera/CameraComponent.h"

#include "WorldCharacter.generated.h"

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
     * A wrapper around primitive components that only should be visible for other players or the owing player
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

#pragma region Member Variables

    float DefaultFieldOfView = 120.0f;
    float ZoomedFieldOfView  = 60.0f;

#pragma endregion Member Variables

#pragma region Enhanced Input

public:

    //////////////////////////////////////////////////////////////////////////
    // Enhanced Input Extern Events
    //////////////////////////////////////////////////////////////////////////

private:

    FOnCamerChangedEventSignature OnCameraChangedEvent;

protected:

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    /** Override this method to add custom key bindings in derived classes. */
    virtual auto BindAction(const FString& ActionName, UEnhancedInputComponent* EnhancedInputComponent) -> void;

    virtual void OnMove(const FInputActionValue& Value);
    virtual void OnLook(const FInputActionValue& Value);
    virtual void OnStartedJump(const FInputActionValue& Value);
    virtual void OnTriggerJump(const FInputActionValue& Value);
    virtual void OnCompleteJump(const FInputActionValue& Value);
    virtual void OnTriggerCrouch(const FInputActionValue& Value);
    virtual void OnCompleteCrouch(const FInputActionValue& Value);

    virtual void OnToggleCameras(const FInputActionValue& Value);
    virtual void OnTriggerZoomFirstPersonCamera(const FInputActionValue& Value);
    virtual void OnCompleteZoomFirstPersonCamera(const FInputActionValue& Value);

    float LastJumpTimeInFlyMode = 0.0f;
    static constexpr float JumpFlyModeDeactivationTime = 0.5f;

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

#pragma region World Interaction

public:

    FORCEINLINE auto GetFPSCamera(void) const -> UCameraComponent* { return this->FirstPersonCameraComponent; }

    //////////////////////////////////////////////////////////////////////////
    // World Locations And Rotations
    //////////////////////////////////////////////////////////////////////////

    /** The added offset to the First-Person Camera Component. */
    inline static const FVector TorsoOffset { 0.0f, 0.0f, -50.0f };

    FORCEINLINE auto GetTorsoLocation(void) const -> FVector { return this->FirstPersonCameraComponent->GetComponentLocation() + AWorldCharacter::TorsoOffset; }
    FORCEINLINE auto GetTorsoRelativeLocation(void) const -> FVector { return this->FirstPersonCameraComponent->GetRelativeLocation() + AWorldCharacter::TorsoOffset; }
    FORCEINLINE auto GetTorsoTransform(void) const -> FTransform { return FTransform(this->FirstPersonCameraComponent->GetComponentRotation(), this->GetTorsoLocation()); }

    FORCEINLINE auto GetFeetLocation(void) const -> FVector { return this->GetActorLocation(); }

#pragma endregion World Interaction

};
