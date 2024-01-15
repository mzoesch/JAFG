// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "HUD/UW_Master.h"

#include "UW_DebugScreen.generated.h"

UCLASS()
class JAFG_API UUW_DebugScreen : public UUW_Master
{
	GENERATED_BODY()

#pragma region Debug API

public:

	static float GAverageFPS();
	static float GetMaxFPS();
	
#pragma endregion Debug API
	
#pragma region Debug Screen Sections

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Debug")
	FString GetSectionFPS() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Debug")
	FString GetSectionClientCharacterLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Debug")
	FString GetSectionClientCharacterChunkLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Debug")
	FString GetSectionClientCharacterFacing() const;
	
#pragma endregion Debug Screen Sections

};
