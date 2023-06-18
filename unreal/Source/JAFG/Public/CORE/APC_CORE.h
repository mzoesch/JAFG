// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"

#include "../World/FJAFGCoordinateSystem.h"

#include "APC_CORE.generated.h"


class UInputMappingContext;
class UInputAction;
class AACTR_BLOCKCORE;

UCLASS()
class JAFG_API APC_CORE : public APlayerController {
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;	

#pragma region Input Mappings

public:

	// IMC

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input|IA|IMC")
	UInputMappingContext* IMC_DefaultStart;

	// Movement

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
	UInputAction* IA_Jump;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
	UInputAction* IA_Look;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
	UInputAction* IA_Move;
	
	// Interaction

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Interaction")
	UInputAction* IA_Primary;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Interaction")
	UInputAction* IA_Secondary;

	// Debug

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Debug")
	UInputAction* IA_ToggleDebugScreen;

#pragma endregion Input Mappings

public:
	
	// Call this function from the character
	bool SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

#pragma region Input Actions

private:

	// Movement
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void TriggerJump(const FInputActionValue& Value);
	void CompleteJump(const FInputActionValue& Value);

	// Interaction
	void Primary();
	void Secondary();

	// Debug
	void ToggleDebugScreen();

#pragma endregion Input Actions

#pragma region Server Functions

public:

	UFUNCTION(Server, Reliable)
	void SV_DestroyBlock(const FTransform& StartTransform);
	UFUNCTION(Server, Reliable)
	void SV_PlaceBlock(const FJAFGCoordinateSystem& BlockPosition);

#pragma endregion Server Functions

#pragma region Player State API

public:

	FVector GetPlayerPosition() const;

#pragma endregion Player State API

	// Just Temporary
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AACTR_BLOCKCORE> BlockClass;

};
