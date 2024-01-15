// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "Lib/FAccumulated.h"

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
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Components

public:

    /* TODO Are these params necessary? */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCameraComponent;

    UPROPERTY()
    ACuboid* ItemPreview;
    
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
    void OnQuickSlotSelect(const int Slot);

    /* MISC */
    void OnDebugScreenToggle(const FInputActionValue& Value);
    
#pragma endregion Input Actions

#pragma region Static Meshes

public:
    
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Mesh")
    const TObjectPtr<UMaterialInterface> DevMaterial;
    
#pragma endregion Static Meshes
    
#pragma region Member Methods, Variables

#pragma region Inventory
    
public:

    int QuickSlotSelected;

private:
    
    TArray<FAccumulated> Inventory;

public:
    
    FAccumulated GetInventorSlot(const int Slot) const { return this->Inventory[Slot]; }
    FAccumulated GetQuickSlotSelected() const { return this->Inventory[this->QuickSlotSelected]; }
    bool AddToInventory(const FAccumulated Accumulated);

private:

    void AddToInventorySlot(const int Slot, const uint16_t Amount);
    void AddToInventorySlot(const int Slot, const FAccumulated Accumulated);
    
#pragma endregion Inventory
    
#pragma endregion Member Methods, Variables

#pragma region Getters

public:

    UCameraComponent* GetFPSCamera() const;
    
#pragma endregion Getters
    
};
