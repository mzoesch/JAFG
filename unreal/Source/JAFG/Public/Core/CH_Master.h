// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"

#include "Lib/FAccumulated.h"
#include "Lib/PrescriptionSeeker.h"
#include "Lib/Container/Slot.h"
#include "World/Chunk.h"
#include "Core/PC_Master.h"

#include "CH_Master.generated.h"

class UMasterCameraShake;
class ACharacterReach;
class UBoxComponent;
class AChunk;
class ACuboid;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class AItemPreview;
enum ECrosshairState : uint8;

UCLASS()
class JAFG_API ACH_Master : public ACharacter
{
    GENERATED_BODY()

public:

    ACH_Master();

protected:

    virtual void BeginPlay(void) override;

public:	

    virtual void Tick(const float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

#pragma region Components

public:
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCameraComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    ACuboid* ItemPreview;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<ACharacterReach> CharacterReachClass;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    ACharacterReach* CharacterReach;
    
#pragma endregion Components
    
#pragma region Input Mappings
    
public:

    /* IMC */

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC")
    UInputMappingContext* IMCFoot;

    /* TODO Rename to IMC_CONTAINER */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC")
    UInputMappingContext* IMCContainer;
    
    /* Movement */

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
    UInputAction* IAJump;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
    UInputAction* IALook;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
    UInputAction* IAMove;

    /* Interaction */

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Interaction")
    UInputAction* IAPrimary;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Interaction")
    UInputAction* IASecondary;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Interaction")
    UInputAction* IADropAccumulated;
    
    /* Inventory */

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAToggleInventory;
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAQuickSlot0;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAQuickSlot1;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAQuickSlot2;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAQuickSlot3;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAQuickSlot4;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAQuickSlot5;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAQuickSlot6;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAQuickSlot7;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAQuickSlot8;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAQuickSlot9;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Inventory")
    UInputAction* IAQuickSlotBitwise;

    /* MISC */

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|MISC")
    UInputAction* IADebugScreen;
    
#pragma endregion Input Mappings

#pragma region Input Actions

private:

    /* Movement */
    void OnMove(const FInputActionValue& Value);
    void OnLook(const FInputActionValue& Value);
    void OnTriggerJump(const FInputActionValue& Value);
    void OnCompleteJump(const FInputActionValue& Value);

    /* Interaction */
    void OnPrimary(const FInputActionValue& Value);
    void OnPrimaryCompleted(const FInputActionValue& Value);
    void OnSecondary(const FInputActionValue& Value);
    void OnDropAccumulated(const FInputActionValue& Value);
    
    /* Inventory */
    void OnInventoryToggle(const FInputActionValue& Value);
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

    /* MISC */
    void OnDebugScreenToggle(const FInputActionValue& Value);
    
#pragma endregion Input Actions
    
#pragma region Member Methods, Variables

public:

    inline static const FVector ItemPreviewVoxelLocationOffset   { 48.0f, 33.5f, -30.0f };
    inline static const FVector ItemPreviewNoVoxelLocationOffset { 55.0f, 24.0f, -10.0f };
    
    /** Will update the current item preview with the current selected quick slot.  */
    void UpdateItemPreview(void) const;
    /** Will not trigger a rerender of the current applied mesh. */
    void UpdateItemPreview(const float Bob) const;

private:

    /**
     * Manages how input mapping context are distributed while containers are open.
     * This method should only be called by the supervising player controller.
     */
    void TransitCharacterToContainerState(const bool bOpen) const;
    friend void APC_Master::TransitToContainerState(const FString& Identifier, const bool bOpen, const bool bDestroy);
    
#pragma region World Interaction
    
private:

    /** Equivalent to 128 Voxels. */
    static constexpr float MaxPOVLineTraceLength { (AChunk::CHUNK_SIZE * 4.0f ) * 100.0f };
    /** TODO In the future the reach will be determined by various factors. */
    // ReSharper disable once CppMemberFunctionMayBeStatic
    FORCEINLINE float GetCharacterReachInVoxels(void) const { return 4.5f; }
    FORCEINLINE float GetCharacterReach(void) const { return this->GetCharacterReachInVoxels() * 100.0f; }

private:    float       CurrentDurationSameVoxelIsMined;
public:     FORCEINLINE float GetCurrentDurationSameVoxelIsMined(void) const { return this->CurrentDurationSameVoxelIsMined; }
private:    FIntVector  CurrentlyMiningLocalVoxelLocation;
public:     FORCEINLINE FIntVector GetCurrentlyMiningLocalVoxelLocation(void) const { return this->CurrentlyMiningLocalVoxelLocation; }
    
#pragma endregion World Interaction
    
#pragma region Inventory

public:

    static constexpr int HotbarSlotCount { 10 };
    
private:

    
    static constexpr int InventoryStartSize    { 78 };
    static constexpr int InventoryCrafterWidth {  2 };
    static constexpr int InventoryCrafterSize  {  4 };

    FString InventoryContainerIdentifier;

    int             SelectedQuickSlotIndex;
public:     FAccumulated    CursorHand;
private:    TArray<FSlot>   Inventory;
    TArray<FSlot>   InventoryCrafter;

#pragma region Inventory Manipulation
    
public:

    //
    // It is generally safe to assume that all methods that do not have
    // a bUpdateHUD parameter boolean will never trigger a re-render.
    //
    // Note even if the method has Inventory Slot in its name it does not mean that the character
    // inventory is open and operated on. Any other container can call these methods as well if
    // they have access to the character inventory.
    //
    
    bool                        IsAContainerVisible(void) const;

    void                        ClearCursorHand(const bool bUpdateHUD);
    
    FORCEINLINE int             GetInventorySize(void) const { return this->Inventory.Num(); }
    FORCEINLINE FAccumulated    GetInventorySlot(const int Slot) const { return this->Inventory[Slot].Content; }
    bool                        AddToInventory(const FAccumulated Accumulated, const bool bUpdateHUD);

    void                        OnInventorySlotClicked(const int Slot, const bool bUpdateHUD);
    void                        OnInventorySlotSecondaryClicked(const int Slot, const bool bUpdateHUD);
    void                        OnInventoryCrafterSlotClicked(const int Slot, const bool bUpdateHUD);
    void                        OnInventoryCrafterSlotSecondaryClicked(const int Slot, const bool bUpdateHUD);
    void                        OnInventoryCrafterProductClicked(const bool bUpdateHUD);
    
    FORCEINLINE int             GetSelectedQuickSlotIndex(void) const { return this->SelectedQuickSlotIndex; }
    FORCEINLINE FAccumulated    GetSelectedQuickSlot(void) const { return this->Inventory[this->SelectedQuickSlotIndex].Content; }
    FORCEINLINE FSlot*          GetSelectedQuickSlotPtr(void) { return &this->Inventory[this->SelectedQuickSlotIndex]; }

    FORCEINLINE int             GetInventoryCrafterSize(void) const { return this->InventoryCrafter.Num(); }
    FORCEINLINE FSlot*          GetInventoryCrafterSlotPtr(const int Slot) { return &this->InventoryCrafter[Slot]; }
    FORCEINLINE FAccumulated    GetInventoryCrafterSlot(const int Slot) const { return this->InventoryCrafter[Slot].Content; }
    FAccumulated                GetInventoryCrafterProduct(void) const;
    FORCEINLINE FDelivery       GetInventoryCrafterAsDelivery(void) const
    {
        TArray<FAccumulated> DeliveryContents;
        for (const FSlot& Slot : this->InventoryCrafter) { DeliveryContents.Add(FAccumulated(Slot.Content.Accumulated)); }
        return FDelivery { DeliveryContents, ACH_Master::InventoryCrafterWidth };
    }
    void                        ClearInventoryCrafterSlots(const bool bUpdateHUD);
    
private:
    
    void AddToInventoryAtSlot(const int Slot, const int Amount, const bool bUpdateHUD);
    void AddToInventoryCrafterAtSlot(const int Slot, const int Amount, const bool bUpdateHUD);
    
    /**
     * In the character crafter we obviously have to remove the accumulated items
     * that where needed to craft such accumulated item.
     * 
     * It is always assumed by the method that the crafter had enough accumulated items to craft the product.
     * The product must be obtained before calling this method as after this method call the product can no longer
     * be determined by the crafter.
     */
    void ReduceInventoryCrafterByProductIngredients(const bool bUpdateHUD);
    
#pragma endregion Inventory Manipulation
    
#pragma endregion Inventory

#pragma region Camera

private:

    UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivate))
    TSubclassOf<UCameraShakeBase> BobClass;

    float BobProgress = 0.0f;

    float OldRealBob = 0.0f;
    float PreviousDistPct;
    
    void TickBob(void);
    
#pragma endregion Camera

#pragma region MISC
    
private:

    /** The added offset to the First Person Camera Component. */
    inline static const FVector TorsoOffset { 0.0f, 0.0f, -50.0f };

#pragma endregion MISC
    
#pragma endregion Member Methods, Variables

#pragma region Getters

public:

    FORCEINLINE UCameraComponent* GetFPSCamera(void) const { return this->FirstPersonCameraComponent; }
    FORCEINLINE FVector GetTorsoLocation(void) const { return this->FirstPersonCameraComponent->GetComponentLocation() + ACH_Master::TorsoOffset /* - FVector(0.0f, 0.0f, 50.0f) */; }
    FORCEINLINE FTransform GetTorsoTransform(void) const { return FTransform(this->FirstPersonCameraComponent->GetComponentRotation(), this->GetTorsoLocation()); }
    void GetTargetedVoxel(AChunk*& OutChunk, FVector& OutWorldHitLocation, FVector_NetQuantizeNormal& OutWorldNormalHitLocation, FIntVector& OutLocalHitVoxelLocation, const float UnrealReach = ACH_Master::MaxPOVLineTraceLength) const;

private:

    FORCEINLINE FTransform GetFirstPersonTraceStart(void) const { return this->FirstPersonCameraComponent->GetComponentTransform(); }
    
#pragma endregion Getters
    
};
