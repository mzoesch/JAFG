// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "ACH_CORE.generated.h"


class UInputMappingContext;
class UInputAction;


UCLASS()
class JAFG_API ACH_CORE : public ACharacter {
	GENERATED_BODY()

public:
	
	// Sets default values for this character's properties
	ACH_CORE();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	// Components
	
	UPROPERTY(EditDefaultsOnly, category = "Components")
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(EditDefaultsOnly, category = "Components")
	class UStaticMeshComponent* PlayerFeet;

public:

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void TriggerJump();
	void CompleteJump();

protected:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
