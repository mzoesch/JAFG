// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "CH_Master.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
#pragma endregion Components
	
public:
	
#pragma region Input Mappings
	
public:

	/* IMC */

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|IMC")
	UInputMappingContext* IMCFoot;

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
	
#pragma endregion Input Actions
	
};
