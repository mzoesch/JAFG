// © 2023 mzoesch. All rights reserved.


#include "CORE/APC_CORE.h"

#include "CORE/ACH_CORE.h"
#include "CORE/AHUD_CORE.h"
#include "CORE/AACTR_BLOCKCORE.h"

#include "World/WorldManipulation.h"
#include "World/Blocks.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

void APC_CORE::BeginPlay() {
	Super::BeginPlay();

	this->bReplicates = true;

	this->PlayerHotbar.Add(Blocks::Stone);
	this->PlayerHotbar.Add(Blocks::Dirt);
	this->PlayerHotbar.Add(Blocks::Grass);
	this->PlayerHotbar.Add(Blocks::Sand);
	this->PlayerHotbar.Add(Blocks::Gravel);

	this->PlayerHotbar.Add(Blocks::OakLeaves);
	this->PlayerHotbar.Add(Blocks::OakWood);
	this->PlayerHotbar.Add(Blocks::Iron);
	this->PlayerHotbar.Add(Blocks::Gold);
	this->PlayerHotbar.Add(Blocks::Base);

	return;
}

void APC_CORE::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(APC_CORE, CurrentHotbarIndex);
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

	// Inventory
	PEI->BindAction(this->IA_QuickSelect0, ETriggerEvent::Started, this, &APC_CORE::QuickSelect0);
	PEI->BindAction(this->IA_QuickSelect1, ETriggerEvent::Started, this, &APC_CORE::QuickSelect1);
	PEI->BindAction(this->IA_QuickSelect2, ETriggerEvent::Started, this, &APC_CORE::QuickSelect2);
	PEI->BindAction(this->IA_QuickSelect3, ETriggerEvent::Started, this, &APC_CORE::QuickSelect3);
	PEI->BindAction(this->IA_QuickSelect4, ETriggerEvent::Started, this, &APC_CORE::QuickSelect4);
	PEI->BindAction(this->IA_QuickSelect5, ETriggerEvent::Started, this, &APC_CORE::QuickSelect5);
	PEI->BindAction(this->IA_QuickSelect6, ETriggerEvent::Started, this, &APC_CORE::QuickSelect6);
	PEI->BindAction(this->IA_QuickSelect7, ETriggerEvent::Started, this, &APC_CORE::QuickSelect7);
	PEI->BindAction(this->IA_QuickSelect8, ETriggerEvent::Started, this, &APC_CORE::QuickSelect8);
	PEI->BindAction(this->IA_QuickSelect9, ETriggerEvent::Started, this, &APC_CORE::QuickSelect9);

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

#pragma region Inventory


void APC_CORE::QuickSelect0() {
	this->SV_QuickSelect(0);
	if (AHUD_CORE* HUD_Core = CastChecked<AHUD_CORE>(this->GetHUD()))
		HUD_Core->UpdateHotBar(0);
	return;
}

void APC_CORE::QuickSelect1() {
	this->SV_QuickSelect(1);
	if (AHUD_CORE* HUD_Core = CastChecked<AHUD_CORE>(this->GetHUD()))
		HUD_Core->UpdateHotBar(1);
	return;
}

void APC_CORE::QuickSelect2() {
	this->SV_QuickSelect(2);
	if (AHUD_CORE* HUD_Core = CastChecked<AHUD_CORE>(this->GetHUD()))
		HUD_Core->UpdateHotBar(2);
	return;
}

void APC_CORE::QuickSelect3() {
	this->SV_QuickSelect(3);
	if (AHUD_CORE* HUD_Core = CastChecked<AHUD_CORE>(this->GetHUD()))
		HUD_Core->UpdateHotBar(3);
	return;
}

void APC_CORE::QuickSelect4() {
	this->SV_QuickSelect(4);
	if (AHUD_CORE* HUD_Core = CastChecked<AHUD_CORE>(this->GetHUD()))
		HUD_Core->UpdateHotBar(4);
	return;
}

void APC_CORE::QuickSelect5() {
	this->SV_QuickSelect(5);
	if (AHUD_CORE* HUD_Core = CastChecked<AHUD_CORE>(this->GetHUD()))
		HUD_Core->UpdateHotBar(5);
	return;
}

void APC_CORE::QuickSelect6() {
	this->SV_QuickSelect(6);
	if (AHUD_CORE* HUD_Core = CastChecked<AHUD_CORE>(this->GetHUD()))
		HUD_Core->UpdateHotBar(6);
	return;
}

void APC_CORE::QuickSelect7() {
	this->SV_QuickSelect(7);
	if (AHUD_CORE* HUD_Core = CastChecked<AHUD_CORE>(this->GetHUD()))
		HUD_Core->UpdateHotBar(7);
	return;
}

void APC_CORE::QuickSelect8() {
	this->SV_QuickSelect(8);
	if (AHUD_CORE* HUD_Core = CastChecked<AHUD_CORE>(this->GetHUD()))
		HUD_Core->UpdateHotBar(8);
	return;
}

void APC_CORE::QuickSelect9() {
	this->SV_QuickSelect(9);
	if (AHUD_CORE* HUD_Core = CastChecked<AHUD_CORE>(this->GetHUD()))
		HUD_Core->UpdateHotBar(9);
	return;
}

void APC_CORE::SV_QuickSelect_Implementation(int Index) {
	this->CurrentHotbarIndex = Index;
	return;
}

#pragma endregion Inventory

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
	
	WorldManipulation::SpawnBlock(
		this->GetWorld(),
		BlockPosition,
		this->GetCurrentlySelectedBlock()
	);

	return;
}

#pragma endregion Server Functions

#pragma region Player State API

FVector APC_CORE::GetPlayerPosition() const {
	return CastChecked<ACH_CORE>(this->GetPawn())->PlayerFeet->GetComponentLocation();
}

FString APC_CORE::GetCurrentlySelectedBlock() const {
	return this->PlayerHotbar[this->CurrentHotbarIndex];
}

#pragma endregion Player State API
