// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"

#include "Lib/FAccumulated.h"
#include "Lib/PrescriptionSeeker.h"
#include "Lib/Container/Slot.h"

#include "CH_Master.generated.h"

class ACuboid;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class AItemPreview;

UCLASS()
class JAFG_API ACH_Master : public ACharacter
{
    GENERATED_BODY()

public:

    ACH_Master();

protected:

    virtual void BeginPlay() override;

public:	

    virtual void Tick(const float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

#pragma region Components

public:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent*   FirstPersonCameraComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    ACuboid*            ItemPreview;
    
#pragma endregion Components
    
#pragma region Input Mappings
    
public:

    /* IMC */

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC")
    UInputMappingContext* IMCFoot;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC")
    UInputMappingContext* IMCInventory;
    
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
    void OnQuickSlot(const int Slot);

    /* MISC */
    void OnDebugScreenToggle(const FInputActionValue& Value);
    
#pragma endregion Input Actions
    
#pragma region Member Methods, Variables

#pragma region Inventory
    
private:
    

    static constexpr int    InventoryStartSize{78};
    static constexpr int    InventoryCrafterWidth{2};
    static constexpr int    InventoryCrafterSize{4};

    int             SelectedQuickSlotIndex;
public:     FAccumulated    CursorHand;
private:    TArray<FSlot>   Inventory;
    TArray<FSlot>   InventoryCrafter;
    FAccumulated    InventoryCrafterProduct;

#pragma region Inventory Manipulation
    
public:

    //
    // It is generally safe to assume that all methods that do not have
    // a bUpdateHUD parameter boolean will never trigger a re-render.
    //
    
    bool                        IsInventoryOpen() const;

    void                        ClearCursorHand(const bool bUpdateHUD);
    
    FORCEINLINE int             GetInventorySize() const { return this->Inventory.Num(); }
    FORCEINLINE FAccumulated    GetInventorySlot(const int Slot) const { return this->Inventory[Slot].Content; }
    bool                        AddToInventory(const FAccumulated Accumulated, const bool bUpdateHUD);

    void                        OnInventorySlotClicked(const int Slot, const bool bUpdateHUD);
    void                        OnInventorySlotSecondaryClicked(const int Slot, const bool bUpdateHUD);
    void                        OnInventoryCrafterSlotClicked(const int Slot, const bool bUpdateHUD);
    void                        OnInventoryCrafterSlotSecondaryClicked(const int Slot, const bool bUpdateHUD);
    void                        OnInventoryCrafterProductClicked(const bool bUpdateHUD);
    
    FORCEINLINE int             GetSelectedQuickSlotIndex() const { return this->SelectedQuickSlotIndex; }
    FORCEINLINE FAccumulated    GetSelectedQuickSlot() const { return this->Inventory[this->SelectedQuickSlotIndex].Content; }
    FORCEINLINE FSlot*          GetSelectedQuickSlotPtr() { return &this->Inventory[this->SelectedQuickSlotIndex]; }

    FORCEINLINE int             GetInventoryCrafterSize() const { return this->InventoryCrafter.Num(); }
    FORCEINLINE FAccumulated    GetInventoryCrafterSlot(const int Slot) const { return this->InventoryCrafter[Slot].Content; }
    FAccumulated                GetInventoryCrafterProduct() const;
    FORCEINLINE FDelivery       GetInventoryCrafterAsDelivery() const
    {
        TArray<FAccumulated> DeliveryContents; for (const FSlot& S : this->InventoryCrafter) { DeliveryContents.Add(S.Content); }
        return FDelivery { DeliveryContents, ACH_Master::InventoryCrafterWidth };
    }
    void                        ClearInventoryCrafterSlots(const bool bUpdateHUD);
    
private:
    
    void AddToInventoryAtSlot(const int Slot, const int Amount, const bool bUpdateHUD);
    void AddToInventoryCrafterAtSlot(const int Slot, const int Amount, const bool bUpdateHUD);
    
    /** In the character crafter we obviously have to remove the accumulated items that where needed to craft such item. */
    void ReduceInventoryCrafterByProductIngredients(const bool bUpdateHUD);
    
#pragma endregion Inventory Manipulation
    
#pragma endregion Inventory
    
#pragma endregion Member Methods, Variables

#pragma region Getters

public:

    FORCEINLINE UCameraComponent* GetFPSCamera() const { return this->FirstPersonCameraComponent; }
    FORCEINLINE FVector GetTorsoLocation() const { return this->FirstPersonCameraComponent->GetComponentLocation() - FVector(0, 0, 50); }
    FORCEINLINE FTransform GetTorsoTransform() const { return FTransform(this->FirstPersonCameraComponent->GetComponentRotation(), this->GetTorsoLocation()); }
    
#pragma endregion Getters
    
};
