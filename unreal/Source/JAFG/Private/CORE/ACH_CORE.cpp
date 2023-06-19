// © 2023 mzoesch. All rights reserved.


#include "CORE/ACH_CORE.h"

#include "CORE/APC_CORE.h"
#include "CORE/AHUD_CORE.h"

#include "Camera/CameraComponent.h"
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

	this->FirstPersonCamera =
		CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	this->FirstPersonCamera->SetupAttachment(this->GetCapsuleComponent());
	this->FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	this->FirstPersonCamera->bUsePawnControlRotation = true;

	this->PlayerFeet = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerFeet"));
	this->PlayerFeet->SetupAttachment(RootComponent);
	this->PlayerFeet->SetRelativeLocation(FVector(0.f, 0.f, -96.f));

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
	UE_LOG(LogTemp, Warning, TEXT("WARNING: ACH_CORE::Tick should never be called."));
	return;
}

void ACH_CORE::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	APC_CORE* PC_Core = CastChecked<APC_CORE>(GetController());
	check(PC_Core);
	bool bSuccessfull = PC_Core->SetupPlayerInputComponent(PlayerInputComponent);
	
	if (bSuccessfull) {
		UE_LOG(LogTemp, Display, TEXT("ACH_CORE::SetupPlayerInputComponent was successfully executed."))
		return;
	}

	UE_LOG(LogTemp, Fatal, TEXT("ACH_CORE::SetupPlayerInputComponent executed with errors."))
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

FTransform ACH_CORE::GetCameraTransfrom() {
	return this->FirstPersonCamera->GetComponentTransform();
}

FHitResult ACH_CORE::TraceNearestObject(float JAFGRange) {
	const FVector CameraLocation = this->FirstPersonCamera->GetComponentLocation();
	const FRotator CameraRotation = this->FirstPersonCamera->GetComponentRotation();
	
	return this->TraceNearestObject(JAFGRange, FTransform(CameraRotation, CameraLocation));
}

FHitResult ACH_CORE::TraceNearestObject(float JAFGRange, const FTransform& StartTransform) {
	const FVector TraceEnd =
		StartTransform.GetLocation() + (StartTransform.GetRotation().Vector()
			* (JAFGRange * FJAFGCoordinateSystem::JAFGToUnrealCoordinateSystemScale)
			)
			;

	FCollisionQueryParams TraceParams =
		FCollisionQueryParams(FName(TEXT("")), false, this->GetOwner());

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartTransform.GetLocation(),
		TraceEnd,
		ECC_Visibility,
		TraceParams
		)
		;

	return HitResult;
}
