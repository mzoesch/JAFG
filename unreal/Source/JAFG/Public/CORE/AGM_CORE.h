// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "AGM_CORE.generated.h"


UCLASS()
class JAFG_API AGM_CORE : public AGameMode {
	GENERATED_BODY()

#pragma region Blocks

public:

	UPROPERTY(EditDefaultsOnly, category = "Actors|Blocks|T0") TSubclassOf<class AACTR_BLOCKCORE> Base;
	UPROPERTY(EditDefaultsOnly, category = "Actors|Blocks|T0") TSubclassOf<class AACTR_BLOCKCORE> Stone;


#pragma endregion Blocks

#pragma region Items

public:

	UPROPERTY(EditDefaultsOnly, category = "Actors|ItemsTextures") UTexture2D* TBase;
	UPROPERTY(EditDefaultsOnly, category = "Actors|ItemsTextures") UTexture2D* TStone;

#pragma endregion Items
	
};
