// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "WorldGeneratorInfo.h"
#include "Camera/CameraComponent.h"
#include "Chunk/ChunkWorldSubsystem.h"
#include "GameFramework/Character.h"
#include "Jar/Slot.h"

#include "WorldCharacter.generated.h"

JAFG_VOID

class ULocalChunkValidator;
class UChatComponent;
class UChatMenu;
class UEscapeMenu;
class UInputAction;
class UInputComponent;
class UInputMappingContext;
struct FInputActionValue;

UCLASS(Abstract, Blueprintable)
class JAFG_API AWorldCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    explicit AWorldCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual auto Tick(const float DeltaTime) -> void override;
    virtual auto GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const -> void override;

private:

    //////////////////////////////////////////////////////////////////////////
    // AActor Components
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UChatComponent> ChatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ULocalChunkValidator> LocalChunkValidator;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

    //////////////////////////////////////////////////////////////////////////
    // Enhanced Input
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> IMCFoot;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> IMCMenu;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> IMCChatMenu;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> IMCInventory;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAJump;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IALook;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAMove;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Interraction", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAPrimary;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Interraction", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IASecondary;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAToggleInventory;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAQuickSlot0;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAQuickSlot1;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAQuickSlot2;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAQuickSlot3;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAQuickSlot4;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAQuickSlot5;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAQuickSlot6;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAQuickSlot7;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAQuickSlot8;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAQuickSlot9;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAQuickSlotBitwise;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|MISC", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAToggleEscapeMenu;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|MISC", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAToggleChatMenu;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|MISC", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IAToggleDebugScreen;

    void OnTriggerJump(const FInputActionValue& Value);
    void OnCompleteJump(const FInputActionValue& Value);
    void OnLook(const FInputActionValue& Value);
    void OnMove(const FInputActionValue& Value);

    void OnPrimary(const FInputActionValue& Value);
    UFUNCTION(Server, Reliable)
    void OnPrimary_ServerRPC(const FInputActionValue& Value);
    void OnSecondary(const FInputActionValue& Value);
    UFUNCTION(Server, Reliable)
    void OnSecondary_ServerRPC(const FInputActionValue& Value);

    void OnToggleInventory(const FInputActionValue& Value);
    void OnQuickSlot0(const FInputActionValue& Value);
    void OnQuickSlot1(const FInputActionValue& Value);
    void OnQuickSlot2(const FInputActionValue& Value);
    void OnQuickSlot3(const FInputActionValue& Value);
    void OnQuickSlot4(const FInputActionValue& Value);
    void OnQuickSlot5(const FInputActionValue& Value);
    void OnQuickSlot6(const FInputActionValue& Value);
    void OnQuickSlot7(const FInputActionValue& Value);
    void OnQuickSlot8(const FInputActionValue& Value);
    void OnQuickSlot9(const FInputActionValue& Value);
    void OnQuickSlotBitwise(const FInputActionValue& Value);
    void OnQuickSlot(const int Slot);

    void OnToggleEscapeMenu(const FInputActionValue& Value);
    friend UEscapeMenu;
    void OnToggleChatMenu(const FInputActionValue& Value);
    friend UChatMenu;
    void OnToggleDebugScreen(const FInputActionValue& Value);

public:

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:

    //////////////////////////////////////////////////////////////////////////
    // Inventory and Hotbar
    //////////////////////////////////////////////////////////////////////////

    inline static constexpr int InventoryStartSize    { 78 };
    inline static constexpr int InventoryCrafterWidth {  2 };
    inline static constexpr int InventoryCrafterSize  {  4 };

    UFUNCTION()
    void OnRep_Inventory( /* void */ );
    UPROPERTY(ReplicatedUsing=OnRep_Inventory)
    TArray<FSlot> Inventory;

    UFUNCTION()
    void OnRep_SelectedQuickSlotIndex( /* void */ );
    UPROPERTY(ReplicatedUsing=OnRep_SelectedQuickSlotIndex)
    int SelectedQuickSlotIndex;
    UFUNCTION(Server, Reliable)
    void SetSelectedQuickSlotIndex_ServerRPC(const int Slot);

    void AddToInventoryAtSlot(const int Slot, const int Amount);

public:

    inline static constexpr int HotbarSize            { 10 };

    FORCEINLINE auto GetSelectedQuickSlotIndex(void) const -> int { return this->SelectedQuickSlotIndex; }

    FORCEINLINE auto GetInventory(void) const -> const TArray<FSlot>& { return this->Inventory; }
    FORCEINLINE auto GetInventorySize(void) const -> int { return this->Inventory.Num(); }
    FORCEINLINE auto GetInventorySlot(const int Slot) const -> FAccumulated { return this->Inventory[Slot].Content; }
    /** Server only. */
    FORCEINLINE auto AddToInventory(const FAccumulated& Accumulated) -> bool;

private:

    //////////////////////////////////////////////////////////////////////////
    // Misc
    //////////////////////////////////////////////////////////////////////////

    /** The added offset to the First Person Camera Component. */
    inline static const FVector TorsoOffset { 0.0f, 0.0f, -50.0f };

    /**
     * APawn#RemoteViewPitch is a uint8 and represents the pitch of the remote view replicated back to the server.
     * This method converts the pitch to degrees.
     * Does not work on listens servers.
     */
    FORCEINLINE auto GetRemoteViewPitchAsDeg(void) const -> float
    {
        return this->RemoteViewPitch * 360.0f / /* 2^8 - 1 = */ 255.0f;
    }

    /**
     * The maximum distance where we trace for voxels and other hit objects.
     * Equivalent to four x Chunk => 4 x 16 Voxels => 64 Voxels.
     */
    static constexpr float MaxPOVLineTraceLength { (ChunkWorldSettings::ChunkSize * 4.0f ) * ChunkWorldSettings::JToUScale };
    // ReSharper disable once CppMemberFunctionMayBeStatic
    FORCEINLINE auto GetCharacterReachInVoxels(void) const -> float { return 4.5f; }
    FORCEINLINE auto GetCharacterReach(void) const -> float { return this->GetCharacterReachInVoxels() * ChunkWorldSettings::JToUScale; }

    FORCEINLINE auto GetFirstPersonTraceStart(void) const -> FTransform { return this->FirstPersonCameraComponent->GetComponentTransform(); }
    FORCEINLINE auto GetFirstPersonTraceStart_DedServer(void) const -> FTransform
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

public:

    void GetTargetedVoxel(
        ACommonChunk*& OutChunk,
        FVector& OutWorldHitLocation,
        FVector_NetQuantizeNormal& OutWorldNormalHitLocation,
        FIntVector& OutLocalHitVoxelLocation,
        const bool bUseRemotePitch,
        const float UnrealReach = AWorldCharacter::MaxPOVLineTraceLength
    ) const;

    FORCEINLINE auto GetFPSCamera(void) const -> UCameraComponent* { return this->FirstPersonCameraComponent; }
    FORCEINLINE auto GetTorsoLocation(void) const -> FVector { return this->FirstPersonCameraComponent->GetComponentLocation() + AWorldCharacter::TorsoOffset; }

    FORCEINLINE auto GetCurrentDurationSameVoxelIsMined(void) const -> float { return 0.0f; }
};
