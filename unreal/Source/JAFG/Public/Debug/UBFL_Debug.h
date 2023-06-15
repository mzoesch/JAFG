// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "UBFL_Debug.generated.h"


class APC_CORE;


UCLASS()
class JAFG_API UBFL_Debug : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
	
#pragma region Debug API

public:

	UFUNCTION(BlueprintPure, BlueprintPure, Category = "JAFG|Debug|CORE")
	static FString ProjectID();

	UFUNCTION(BlueprintPure, BlueprintPure, Category = "JAFG|Debug|CORE")
	static FString ProjectIDFormatted();

	UFUNCTION(BlueprintPure, BlueprintPure, Category = "JAFG|Debug|CORE")
	static FString ProjectVersionNumber();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|Debug|CORE")
	static FString ProjectName();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|Debug|CORE")
	static FString ProjectDescription();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|Debug|CORE")
	static FString ProjectCompanyName();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|Debug|CORE")
	static FString ProjectHomePage();

#pragma endregion Debug API

#pragma region Debug Screen Sections

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|Debug|Screen")
	static FString DebugScreenSectionOne_FPS();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|Debug|Screen")
	static FString DebugScreenSectionTwo_LocationPrecise(APC_CORE* PC_Core);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "JAFG|Debug|Screen")
	static FString DebugScreenSectionTwo_Location(APC_CORE* PC_Core);


#pragma endregion Debug Screen Sections

};
