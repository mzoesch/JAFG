// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "AHUD_CORE.generated.h"


class UJAFG_Widget;


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

private:

	void DefaultBehaviorForShowingTheMouseCursor();
	void DefaultBehaviorForHidingTheMouseCursor();

	void HideWidgetWithNoSideEffects(UJAFG_Widget* Widget);
	void ShowWidgetWithNoSideEffects(UJAFG_Widget* Widget);

#pragma region Input API

public:

	void ToggleDebugScreen();

#pragma endregion Input API

};
