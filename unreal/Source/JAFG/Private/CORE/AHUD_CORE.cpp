// © 2023 mzoesch. All rights reserved.


#include "CORE/AHUD_CORE.h"

#include "HUD/UJAFG_Widget.h"
#include "HUD/UJAFG_Widget_HotBar.h"

#include "CORE/ACH_CORE.h"

#include "Blueprint/UserWidget.h"


AHUD_CORE::AHUD_CORE() {
	PrimaryActorTick.bCanEverTick = false;
	return;
}

void AHUD_CORE::BeginPlay() {
	Super::BeginPlay();

	check(this->DebugScreenClass);
	this->WBP_DebugScreen = CreateWidget<UJAFG_Widget>(this->GetWorld(), this->DebugScreenClass);
	this->WBP_DebugScreen->AddToViewport();
	this->HideWidgetWithNoSideEffects(this->WBP_DebugScreen);

	check(this->GameOverlayClass);
	this->WBP_GameOverlay = CreateWidget<UJAFG_Widget>(this->GetWorld(), this->GameOverlayClass);
	this->WBP_GameOverlay->AddToViewport();
	this->WBP_GameOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);

	check(this->HotBarClass)
	this->WBP_HotBar = CreateWidget<UJAFG_Widget_HotBar>(this->GetWorld(), this->HotBarClass);
	this->WBP_HotBar->AddToViewport();
	this->WBP_HotBar->SetVisibility(ESlateVisibility::HitTestInvisible);

	UE_LOG(LogTemp, Display, TEXT("AHUD_CORE::BeginPlay was successfully executed."));
	return;
}

void AHUD_CORE::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	UE_LOG(LogTemp, Warning, TEXT("WARNING: AHUD_CORE::Tick should never be called."));
	return;
}

void AHUD_CORE::DefaultBehaviorForShowingTheMouseCursor() {
	if (APlayerController* PC = this->GetOwningPlayerController()) {
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI());
		PC->SetIgnoreLookInput(true);

		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("WARNING: AHUD_CORE::DefaultBehaviorForShowingTheMouseCursor() failed to get the owning player.")
	);
	return;
}

void AHUD_CORE::DefaultBehaviorForHidingTheMouseCursor() {
	if (APlayerController* PC = this->GetOwningPlayerController()) {
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
		PC->ResetIgnoreLookInput();

		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("WARNING: AHUD_CORE::DefaultBehaviorForHidingTheMouseCursor() failed to get the owning player.")
	);
	return;
}

void AHUD_CORE::HideWidgetWithNoSideEffects(UJAFG_Widget* Widget) {
	if (Widget) {
		Widget->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("WARNING: AHUD_CORE::HideWidgetWithNoSideEffects() was passed nullptr.")
	);
	return;
}

void AHUD_CORE::ShowWidgetWithNoSideEffects(UJAFG_Widget* Widget) {
	if (Widget) {
		Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("WARNING: AHUD_CORE::ShowWidgetWithNoSideEffects() was passed nullptr.")
	);
	return;
}

#pragma region Input API

void AHUD_CORE::ToggleDebugScreen() {
	if (this->WBP_DebugScreen->IsVisible())
		this->HideWidgetWithNoSideEffects(this->WBP_DebugScreen);
	else
		this->ShowWidgetWithNoSideEffects(this->WBP_DebugScreen);

	return;
}

void AHUD_CORE::UpdateHotBar(int HotBarIndex) {
	this->WBP_HotBar->UpdateSelectedSlot(HotBarIndex);
	return;
}

#pragma endregion Input API
