// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "APC_MainMenu.generated.h"


UCLASS()
class JAFG_API APC_MainMenu : public APlayerController {
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void ShowLoadingScreen(const FText& AdditionInformation);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowErrorMessage(const FText& ErrorMessage);

};
