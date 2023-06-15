// � 2023 mzoesch. All rights reserved.


#include "CORE/APC_CORE.h"

#include "CORE/ACH_CORE.h"
#include "CORE/AHUD_CORE.h"

#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void APC_CORE::BeginPlay() {
	Super::BeginPlay();
	
	return;
}

bool APC_CORE::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer());
	check(Subsystem)	
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(this->IMC_DefaultStart, 0);

	UEnhancedInputComponent* PEI = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// Movement
	PEI->BindAction(this->IA_Move, ETriggerEvent::Triggered, this, &APC_CORE::Move);
	PEI->BindAction(this->IA_Look, ETriggerEvent::Triggered, this, &APC_CORE::Look);
	PEI->BindAction(this->IA_Jump, ETriggerEvent::Triggered, this, &APC_CORE::TriggerJump);
	PEI->BindAction(this->IA_Jump, ETriggerEvent::Completed, this, &APC_CORE::CompleteJump);

	// Debug
	PEI->BindAction(this->IA_ToggleDebugScreen, ETriggerEvent::Started, this, &APC_CORE::ToggleDebugScreen);

	return true;
}

#pragma region Input Actions

#pragma region Movement

void APC_CORE::Move(const FInputActionValue& Value) {
	if (ACH_CORE* PC_Core = Cast<ACH_CORE>(this->GetPawn()))
		PC_Core->Move(Value);

	return;
}

void APC_CORE::Look(const FInputActionValue& Value) {
	if (ACH_CORE* PC_Core = Cast<ACH_CORE>(GetPawn()))
		PC_Core->Look(Value);

	return;
}

void APC_CORE::TriggerJump(const FInputActionValue& Value) {
	if (ACH_CORE* PC_Core = Cast<ACH_CORE>(GetPawn()))
		PC_Core->TriggerJump();

	return;
}

void APC_CORE::CompleteJump(const FInputActionValue& Value) {
	if (ACH_CORE* PC_Core = Cast<ACH_CORE>(GetPawn()))
		PC_Core->CompleteJump();

	return;
}

#pragma endregion Movement

#pragma region Debug

void APC_CORE::ToggleDebugScreen() {
	if (AHUD_CORE* HUD_Core = Cast<AHUD_CORE>(this->GetHUD()))
		HUD_Core->ToggleDebugScreen();


	return;
}

#pragma endregion Debug

#pragma endregion Input Actions

#pragma region Player State API

FVector APC_CORE::GetPlayerPosition() const {
	return CastChecked<ACH_CORE>(this->GetPawn())->PlayerFeet->GetComponentLocation();
}

#pragma endregion Player State API
