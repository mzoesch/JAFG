// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "FSearchResult.h"
#include "FLocalSave.h"

#include "APC_MainMenu.generated.h"


UCLASS()
class JAFG_API APC_MainMenu : public APlayerController {
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void ShowLoadingScreen(const FText& AdditionInformation);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowErrorMessage(const FText& ErrorMessage);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSearchSessionsComplete(const TArray<FSearchResult>& SearchResults);

public:

	UFUNCTION(BlueprintCallable)
	static TArray<FLocalSave> GetAllLocalSaves();

	UFUNCTION(BlueprintCallable)
	static int DeleteLocalSave(const FString& SaveName);
	
};
