// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "HUD/UW_Master.h"

#include "DebugScreen.generated.h"

UCLASS()
class JAFG_API UW_DebugScreen : public UW_Master
{
	GENERATED_BODY()

#pragma region Debug API

public:

	FORCEINLINE static float GAverageFPS() { extern ENGINE_API float GAverageFPS; return GAverageFPS; }
	FORCEINLINE static float GetMaxFPS() { if (const UEngine* Engine = Cast<UEngine>(GEngine)) { return Engine->GetMaxFPS(); } return MAX_flt; }
	
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Debug")
	FString GetSectionTargetVoxelData() const;
	
#pragma endregion Debug Screen Sections

};
