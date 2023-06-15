// © 2023 mzoesch. All rights reserved.


#include "CORE/ACH_CORE.h"

#include "CORE/APC_CORE.h"

#include "Components/CapsuleComponent.h"

// Sets default values
ACH_CORE::ACH_CORE() {

	// Set this character to call Tick() every frame.
	// You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	this->BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	this->BodyMesh->SetupAttachment(RootComponent);
	this->BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, -96.f));

	return;
}

// Called when the game starts or when spawned
void ACH_CORE::BeginPlay() {
	Super::BeginPlay();
	
	return;
}

// Called every frame
void ACH_CORE::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	return;
}

void ACH_CORE::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {

	// APlayerController* PC = Cast<APlayerController>(GetController());
	// APC_CORE* PC_Core = Cast<APC_CORE>(PC);

	if (APC_CORE* PC_Core = CastChecked<APC_CORE>(GetController()))
		PC_Core->SetupPlayerInputComponent(PlayerInputComponent);
	
	return;
}

void ACH_CORE::Move(const FInputActionValue& Value) {
	AddMovementInput(GetActorForwardVector(), Value.Get<FVector2D>().Y);
	AddMovementInput(GetActorRightVector(), Value.Get<FVector2D>().X);

	return;
}

void ACH_CORE::Look(const FInputActionValue& Value) {
	AddControllerYawInput(Value.Get<FVector2D>().X * 0.2);
	AddControllerPitchInput(Value.Get<FVector2D>().Y * -0.2);

	return;
}

void ACH_CORE::TriggerJump() {
	Super::Jump();

	return;
}

void ACH_CORE::CompleteJump() {
	Super::StopJumping();

	return;
}
