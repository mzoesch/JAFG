// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "AHUD_CORE.generated.h"


class UJAFG_Widget;
class UJAFG_Widget_HotBar;


UCLASS()
class JAFG_API AHUD_CORE : public AHUD {
	GENERATED_BODY()

public:

	AHUD_CORE();

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:

	UPROPERTY(EditDefaultsOnly)
	class TSubclassOf<UJAFG_Widget> DebugScreenClass;
	class UJAFG_Widget* WBP_DebugScreen;

	UPROPERTY(EditDefaultsOnly)
	class TSubclassOf<UJAFG_Widget> GameOverlayClass;
	class UJAFG_Widget* WBP_GameOverlay;

	UPROPERTY(EditDefaultsOnly)
	class TSubclassOf<UJAFG_Widget_HotBar> HotBarClass;
	class UJAFG_Widget_HotBar* WBP_HotBar;

private:

	void DefaultBehaviorForShowingTheMouseCursor();
	void DefaultBehaviorForHidingTheMouseCursor();

	void HideWidgetWithNoSideEffects(UJAFG_Widget* Widget);
	void ShowWidgetWithNoSideEffects(UJAFG_Widget* Widget);

#pragma region Input API

public:

	void ToggleDebugScreen();
	void UpdateHotBar(int HotBarIndex);

#pragma endregion Input API

};
