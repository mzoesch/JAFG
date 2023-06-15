// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"

#include "APC_CORE.generated.h"


class UInputMappingContext;
class UInputAction;


UCLASS()
class JAFG_API APC_CORE : public APlayerController {
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	
public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input|IA")
	UInputMappingContext* IMC_DefaultStart;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
	UInputAction* IA_Jump;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
	UInputAction* IA_Look;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|Movement")
	UInputAction* IA_Move;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|MISC")
	UInputAction* IA_EscapeMenu;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|MISC")
	UInputAction* IA_OpenDebugMenu;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IA|MISC")
	UInputAction* IA_OpenGameChat;


public:
	
	// Call this function from the character
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

private:

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void TriggerJump(const FInputActionValue& Value);
	void CompleteJump(const FInputActionValue& Value);

};
