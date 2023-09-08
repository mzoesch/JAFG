// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "BFL_Generic.generated.h"


UCLASS()
class JAFG_API UBFL_Generic : public UBlueprintFunctionLibrary {
public:
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FString GetFullLevelSavePathFromLevelName(const FString LevelName);
	
};
