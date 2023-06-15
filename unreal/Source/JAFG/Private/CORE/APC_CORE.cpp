// © 2023 mzoesch. All rights reserved.


#include "CORE/APC_CORE.h"

#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CORE/ACH_CORE.h"

void APC_CORE::BeginPlay() {
	Super::BeginPlay();
	
	return;
}

void APC_CORE::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	UE_LOG(LogTemp, Warning, TEXT("APC_CORE::SetupPlayerInputComponent()"));

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer());

	UE_LOG(LogTemp, Warning, TEXT("Subsystem: %s"), Subsystem ? TEXT("true") : TEXT("false"));

	if (Subsystem) {
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(this->IMC_DefaultStart, 0);
	}
	
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	UE_LOG(LogTemp, Warning, TEXT("PEI: %s"), PEI ? TEXT("true") : TEXT("false"));

	if (PEI) {
		PEI->BindAction(this->IA_Move, ETriggerEvent::Triggered, this, &APC_CORE::Move);
		PEI->BindAction(this->IA_Look, ETriggerEvent::Triggered, this, &APC_CORE::Look);
		PEI->BindAction(this->IA_Jump, ETriggerEvent::Triggered, this, &APC_CORE::TriggerJump);
		PEI->BindAction(this->IA_Jump, ETriggerEvent::Completed, this, &APC_CORE::CompleteJump);
	}
}

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
