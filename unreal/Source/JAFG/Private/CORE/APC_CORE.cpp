// © 2023 mzoesch. All rights reserved.


#include "CORE/APC_CORE.h"

#include "CORE/ACH_CORE.h"
#include "CORE/AHUD_CORE.h"
#include "CORE/AACTR_BLOCKCORE.h"

#include "Kismet/GameplayStatics.h"
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

	// Interaction
	PEI->BindAction(this->IA_Primary, ETriggerEvent::Started, this, &APC_CORE::Primary);
	PEI->BindAction(this->IA_Secondary, ETriggerEvent::Started, this, &APC_CORE::Secondary);

	// Debug
	PEI->BindAction(this->IA_ToggleDebugScreen, ETriggerEvent::Started, this, &APC_CORE::ToggleDebugScreen);

	return true;
}

#pragma region Input Actions

#pragma region Movement

void APC_CORE::Move(const FInputActionValue& Value) {
	if (ACH_CORE* CH_Core = CastChecked<ACH_CORE>(this->GetPawn()))
		CH_Core->Move(Value);

	return;
}

void APC_CORE::Look(const FInputActionValue& Value) {
	if (ACH_CORE* CH_Core = CastChecked<ACH_CORE>(GetPawn()))
		CH_Core->Look(Value);

	return;
}

void APC_CORE::TriggerJump(const FInputActionValue& Value) {
	if (ACH_CORE* CH_Core = CastChecked<ACH_CORE>(GetPawn()))
		CH_Core->TriggerJump();

	return;
}

void APC_CORE::CompleteJump(const FInputActionValue& Value) {
	if (ACH_CORE* CH_Core = CastChecked<ACH_CORE>(GetPawn()))
		CH_Core->CompleteJump();

	return;
}

#pragma endregion Movement

#pragma region Interaction

void APC_CORE::Primary() {
	// Player wants to destroy block
	ACH_CORE* CH_Core = CastChecked<ACH_CORE>(this->GetPawn());
	if (!CH_Core)
		return;
	FHitResult HitResult = CH_Core->TraceNearestObject(4);
	if (!HitResult.bBlockingHit)
		return;
	AACTR_BLOCKCORE* Block = Cast<AACTR_BLOCKCORE>(HitResult.GetActor());
	if (!Block)
		return;

	// Debug
	// DrawDebugLine(this->GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Red, false, 5.f);
	// DrawDebugSphere(this->GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Red, false, 5.f);
	
	FTransform CameraTransform = CH_Core->GetCameraTransfrom();
	this->SV_DestroyBlock(CameraTransform);
	return;
}

void APC_CORE::Secondary() {
	// Player wants to place block
	ACH_CORE* CH_Core = CastChecked<ACH_CORE>(this->GetPawn());
	if (!CH_Core)
		return;
	FHitResult HitResult = CH_Core->TraceNearestObject(4);
	if (!HitResult.bBlockingHit)
		return;
	AACTR_BLOCKCORE* Block = Cast<AACTR_BLOCKCORE>(HitResult.GetActor());
	if (!Block)
		return;
	
	// Debug
	// DrawDebugLine(this->GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Red, false, 5.f);
	// DrawDebugSphere(this->GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Red, false, 5.f);
	
	FJAFGCoordinateSystem NewBlockPosition = Block->GetNewBlockPositionOnHit(HitResult);
	this->SV_PlaceBlock(NewBlockPosition);

	return;
}

#pragma endregion Interaction

#pragma region Debug

void APC_CORE::ToggleDebugScreen() {
	if (AHUD_CORE* HUD_Core = Cast<AHUD_CORE>(this->GetHUD()))
		HUD_Core->ToggleDebugScreen();

	return;
}

#pragma endregion Debug

#pragma endregion Input Actions

#pragma region Server Functions

void APC_CORE::SV_DestroyBlock_Implementation(const FTransform& StartTransform) {
	ACH_CORE* CH_Core = CastChecked<ACH_CORE>(this->GetPawn());
	if (!CH_Core)
		return;
	FHitResult HitResult = CH_Core->TraceNearestObject(4, StartTransform);
	if (!HitResult.bBlockingHit)
		return;
	AACTR_BLOCKCORE* Block = Cast<AACTR_BLOCKCORE>(HitResult.GetActor());
	if (!Block)
		return;

	// Debug
	// DrawDebugLine(this->GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Red, false, 5.f);
	// DrawDebugSphere(this->GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Red, false, 5.f);

	Block->DestroyBlock();

	return;
}

void APC_CORE::SV_PlaceBlock_Implementation(const FJAFGCoordinateSystem& BlockPosition) {
	UE_LOG(LogTemp, Warning, TEXT("SV_PlaceBlock_Implementation %s"), *BlockPosition.ToString());

	bool bAboard = false;
	TArray<AActor*> PlayerPawns;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), ACharacter::StaticClass(), PlayerPawns);
	for (AActor* PlayerPawn : PlayerPawns) {
		ACH_CORE* CH = Cast<ACH_CORE>(PlayerPawn);
		if (!CH)
			continue;

		// TODO: This is not 100% accurate
		CH->GetActorLocation();
		if (CH->GetActorLocation().Equals(BlockPosition.GetAsUnrealTransform().GetLocation(), 100.f)) {
			bAboard = true;
			break;
		}
	}

	if (bAboard)
		return;

	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	AACTR_BLOCKCORE* NewBlock = this->GetWorld()->SpawnActor<AACTR_BLOCKCORE>(
		this->BlockClass, BlockPosition.GetAsUnrealTransform(), SpawnParams
	);
	NewBlock->SetReplicates(true);

	return;
}

#pragma endregion Server Functions

#pragma region Player State API

FVector APC_CORE::GetPlayerPosition() const {
	return CastChecked<ACH_CORE>(this->GetPawn())->PlayerFeet->GetComponentLocation();
}

#pragma endregion Player State API
