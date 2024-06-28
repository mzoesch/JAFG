// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "Container.h"
#include "MyCore.h"
#include "GameFramework/Character.h"
#include "InputTriggers.h"
#include "Camera/CameraComponent.h"
#include "Foundation/JAFGContainer.h"
#include "Player/WorldPlayerController.h"
#include "UI/WorldHUD.h"
#include "WorldCore/Character/MyCharacterMovementComponent.h"

#include "WorldCharacter.generated.h"

JAFG_VOID

class UCuboidComponent;
class ACuboid;
class ACharacterReach;
class UJAFGInputSubsystem;
class UInputMappingContext;
class ACommonChunk;
class USpringArmComponent;
class UInputComponent;
struct FInputActionValue;

DECLARE_MULTICAST_DELEGATE(FOnCamerChangedEventSignature)

UCLASS(Abstract)
class JAFG_API AWorldCharacter : public ACharacter, public IContainer, public IContainerOwner
{
    GENERATED_BODY()

public:

    explicit AWorldCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // AActor implementation
    virtual void BeginPlay(void) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void Tick(const float DeltaSeconds) override;
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

    const FString RightHandSocketName { TEXT("socket_hand_r") };

    UPROPERTY(EditAnywhere)
    FTransform RightHandSocketTransform;

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ACharacterReach> CharacterReach;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMesh> CharacterReachMesh;
    friend ACharacterReach;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCuboidComponent> AccumulatedPreview;

    FORCEINLINE auto GetPlayerController(void) const -> APlayerController* { return Cast<APlayerController>(this->GetController()); }
    template<class T>
    FORCEINLINE auto GetPlayerController(void) const -> T* { return Cast<T>(this->GetController()); }
    FORCEINLINE auto GetWorldPlayerController(void) const -> AWorldPlayerController* { return this->GetController<AWorldPlayerController>(); }

    template<class T>
    FORCEINLINE auto GetHUD(void) const -> T* { return Cast<T>(this->GetPlayerController()->GetHUD()); }
    FORCEINLINE auto GetWorldHUD(void) const -> AWorldHUD* { return this->GetWorldPlayerController()->GetHUD<AWorldHUD>(); }
    void SafeUpdateHotbar(void) const;

    /** Only works on server or locally controlled characters. */
    FORCEINLINE auto GetDisplayName(void) const -> FString { return this->GetWorldPlayerController()->GetDisplayName(); }
    /** For remote connections. */
    FORCEINLINE auto GetRemoteDisplayName(void) const -> FString { return this->GetPlayerState<AWorldPlayerState>()->GetPlayerName(); }

public:

    FORCEINLINE auto GetCurrentVelocity(void) const -> FVector { return this->CurrentVelocity; }
    FORCEINLINE auto GetCurrentSpeed(void) const -> float { return this->CurrentSpeed; }

private:

    UFUNCTION(BlueprintPure, Category = "JAFG|Character", meta = (DisplayName = "Get Current Velocity", AllowPrivateAccess = "true"))
    FVector DynamicGetCurrentVelocity( /* void */ ) const { return this->GetCurrentVelocity(); }

    UFUNCTION(BlueprintPure, Category = "JAFG|Character", meta = (DisplayName = "Get Current Speed", AllowPrivateAccess = "true"))
    float DynamicGetCurrentSpeed( /* void */ ) const { return this->GetCurrentSpeed(); }

protected:

    FVector CurrentVelocity           = FVector::ZeroVector;
    float   CurrentSpeed              = 0.0f;

public:

#pragma region Camera Stuff

    FORCEINLINE auto GetFPSCamera(void) const -> UCameraComponent* { return this->FirstPersonCameraComponent; }

protected:

    float DefaultFieldOfView          = 120.0f;
    float SprintFieldOfViewMultiplier = 1.1f;
    float ZoomedFieldOfView           = 60.0f;
    float DefaultOrthoWidth           = 8192.0f;

    void ReattachAccumulatedPreview(void) const;
    void UpdateAccumulatedPreview(const bool bReattach = false) const;
    FTransform GetAccumulatedPreviewRelativeTransformNoBob(void) const;

#pragma endregion Camera Stuff

#pragma region Container

    const int InventorySize = 30;

public:

    FORCEINLINE auto AsContainer(void) -> IContainer* { return this; }
    FORCEINLINE auto AsContainer(void) const -> const IContainer* { return this; }
    FORCEINLINE auto AsContainerOwner(void) -> IContainerOwner* { return this; }
    FORCEINLINE auto AsContainerOwner(void) const -> const IContainerOwner* { return this; }

    // IContainer interface
    /** Server only. Will handle UI updates, replication, etc. */
    virtual bool EasyAddToContainer(const FAccumulated& Value) override;
    /** Server only. Will handle UI updates, replication, etc. */
    virtual bool EasyChangeContainer(const int32 InIndex, const accamount_t_signed InAmount) override;
    virtual auto ToString_Container(void) const -> FString override;
    // ~IContainer interface

protected:

    UFUNCTION(Server, Reliable, WithValidation)
    void OnContainerChangedEvent_ServerRPC(const ELocalContainerChange::Type InReason, const int32 InIndex);

    // IContainer interface
    FORCEINLINE virtual auto IsContainerInitialized(void) const -> bool override { return this->Container.Num() > 0; }
    FORCEINLINE virtual auto GetContainerSize(void) const -> int32  override { return this->Container.Num(); }
    FORCEINLINE virtual auto GetContainer(void) -> TArray<FSlot>&  override { return this->Container; }
    FORCEINLINE virtual auto GetContainer(void) const -> const TArray<FSlot>&  override { return this->Container; }
    FORCEINLINE virtual auto GetContainer(const int32 Index) -> FSlot&  override { return this->Container[Index]; }
    FORCEINLINE virtual auto GetContainer(const int32 Index) const -> const FSlot&  override { return this->Container[Index]; }
    FORCEINLINE virtual auto GetContainerValue(const int32 Index) const -> FAccumulated  override { return this->Container[Index].Content; }
    FORCEINLINE virtual auto GetContainerValueRef(const int32 Index) -> FAccumulated&  override { return this->Container[Index].Content; }
                virtual auto AddToContainer(const FAccumulated& Value) -> bool  override;
    // ~IContainer interface

private:

    void OnLocalContainerChangedEventImpl(const ELocalContainerChange::Type InReason, const int32 InIndex);

    UPROPERTY(ReplicatedUsing=OnRep_Container)
    TArray<FSlot> Container;
    UFUNCTION()
    void OnRep_Container( /* void */ ) const;

    /** Only meaningful for clients. Holds the last update of this container that was authorized by the server. */
    TArray<FSlot> LastContainerAuth;

#pragma endregion Container

#pragma region Enhanced Input

public:

    FORCEINLINE auto IsFlying(void) const -> bool { return this->GetMyCharacterMovement()->IsFlying(); }
    FORCEINLINE auto IsInputFlyEnabled(void) const -> bool { return this->GetMyCharacterMovement()->bAllowInputFly; }

    //////////////////////////////////////////////////////////////////////////
    // Enhanced Input Extern Events
    //////////////////////////////////////////////////////////////////////////

    auto SubscribeToContainerVisibleEvent(const FOnContainerVisibleSignature::FDelegate& Delegate) -> FDelegateHandle;
    auto UnSubscribeToContainerVisibleEvent(const FDelegateHandle& Handle) -> bool;
    auto SubscribeToContainerLostVisibilityEvent(const FOnContainerLostVisibilitySignature::FDelegate& Delegate) -> FDelegateHandle;
    auto UnSubscribeToContainerLostVisibilityEvent(const FDelegateHandle& Handle) -> bool;

private:

    FOnCamerChangedEventSignature OnCameraChangedEvent;
    FOnContainerVisibleSignature OnContainerVisibleEvent;
    FOnContainerLostVisibilitySignature OnContainerLostVisibilityEvent;

protected:

    /** Shortcut to the JAFG custom movement component. */
    FORCEINLINE auto GetMyCharacterMovement(void) const -> UMyCharacterMovementComponent* { return this->GetCharacterMovement<UMyCharacterMovementComponent>(); }

    virtual auto SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) -> void override;

    auto SetFootContextBasedOnCharacterState(const bool bClearOldMappings = true, const int32 Priority = 0) const -> void;
    auto GetFootContextBasedOnCharacterState(void) const -> UInputMappingContext*;
    auto GetSafeFootContextBasedOnCharacterState(void) const -> UInputMappingContext*;
    auto GetFootContextBasedOnCharacterState(UJAFGInputSubsystem* JAFGInputSubsystem) const -> UInputMappingContext*;
    auto GetSafeFootContextBasedOnCharacterState(UJAFGInputSubsystem* JAFGInputSubsystem) const -> UInputMappingContext*;

    auto BindAction(const FString& ActionName, UEnhancedInputComponent* EnhancedInputComponent) -> void;

    auto OnTriggeredMove(const FInputActionValue& Value) -> void;
    auto OnTriggeredLook(const FInputActionValue& Value) -> void;

    auto OnStartedJump(const FInputActionValue& Value) -> void;
    auto OnTriggerJump(const FInputActionValue& Value) -> void;
    auto OnCompleteJump(const FInputActionValue& Value) -> void;
    float LastJumpStarted = 0.0f;
    static constexpr float JumpFlyModeDeactivationTime = 0.25f;

    auto OnStartedSprint(const FInputActionValue& Value) -> void;
    auto OnCompletedSprint(const FInputActionValue& Value) -> void;

    auto OnTriggerCrouch(const FInputActionValue& Value) -> void;
    auto OnCompleteCrouch(const FInputActionValue& Value) -> void;

    auto OnTriggeredPrimary(const FInputActionValue& Value) -> void;
    auto OnCompletedPrimary(const FInputActionValue& Value) -> void;
    UFUNCTION(Server, Reliable)
    void OnStartedVoxelMinded_ServerRPC(const FIntVector /* FChunkKey */& InTargetedChunk, const FIntVector /* FVoxelKey */& InLocalHitVoxelKey);
    UFUNCTION(Server, Reliable)
    void OnCompletedVoxelMinded_ServerRPC(const bool bClientBreak);
    /** The *local* voxel key. */
    TOptional<FVoxelKey> CurrentlyMiningLocalVoxel       = FVoxelKey::ZeroValue;
    float                CurrentDurationSameVoxelIsMined = 0.0f;

    auto OnStartedSecondary(const FInputActionValue& Value) -> void;
    UFUNCTION(Server, Reliable)
    void OnStartedSecondary_ServerRPC(const FInputActionValue& Value);

    auto OnTriggeredUpMaxFlySpeed(const FInputActionValue& Value) -> void;
    auto OnTriggeredDownMaxFlySpeed(const FInputActionValue& Value) -> void;

    auto OnToggleCameras(const FInputActionValue& Value) -> void;
    auto OnTriggerZoomCameras(const FInputActionValue& Value) -> void;
    auto OnCompleteZoomCameras(const FInputActionValue& Value) -> void;
    auto OnTogglePerspective(const FInputActionValue& Value) -> void;
    bool bZooming = false;

    auto OnStartedToggleContainer(const FInputActionValue& Value) -> void;

    auto OnQuickSlotZero(const FInputActionValue& Value) -> void;
    auto OnQuickSlotOne(const FInputActionValue& Value) -> void;
    auto OnQuickSlotTwo(const FInputActionValue& Value) -> void;
    auto OnQuickSlotThree(const FInputActionValue& Value) -> void;
    auto OnQuickSlotFour(const FInputActionValue& Value) -> void;
    auto OnQuickSlotFive(const FInputActionValue& Value) -> void;
    auto OnQuickSlotSix(const FInputActionValue& Value) -> void;
    auto OnQuickSlotSeven(const FInputActionValue& Value) -> void;
    auto OnQuickSlotEight(const FInputActionValue& Value) -> void;
    auto OnQuickSlotNine(const FInputActionValue& Value) -> void;
    auto OnQuickSlotBitwise(const FInputActionValue& Value) -> void;
    auto OnQuickSlot(const int8 Slot) -> void;
    int8 SelectedQuickSlotIndex = 0;
    UPROPERTY(ReplicatedUsing=OnRep_RemoteSelectedAccumulatedPreview)
    uint32 /* voxel_t */ RemoteSelectedAccumulatedPreview = 0;
    UFUNCTION()
    void OnRep_RemoteSelectedAccumulatedPreview( /* void */ ) const;
    UFUNCTION(Server, Unreliable, WithValidation)
    void OnQuickSlot_ServerRPC(const int8 Slot);
    UFUNCTION(Server, Reliable, WithValidation)
    void OnQuickSlot_ReliableServerRPC(const int8 Slot);

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
