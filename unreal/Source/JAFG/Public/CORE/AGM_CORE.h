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
	UPROPERTY(EditDefaultsOnly, category = "Actors|Blocks|T0") TSubclassOf<class AACTR_BLOCKCORE> Dirt;
	UPROPERTY(EditDefaultsOnly, category = "Actors|Blocks|T0") TSubclassOf<class AACTR_BLOCKCORE> Grass;
	UPROPERTY(EditDefaultsOnly, category = "Actors|Blocks|T0") TSubclassOf<class AACTR_BLOCKCORE> Sand;
	UPROPERTY(EditDefaultsOnly, category = "Actors|Blocks|T0") TSubclassOf<class AACTR_BLOCKCORE> Gravel;
	UPROPERTY(EditDefaultsOnly, category = "Actors|Blocks|T0") TSubclassOf<class AACTR_BLOCKCORE> OakLeaves;
	UPROPERTY(EditDefaultsOnly, category = "Actors|Blocks|T0") TSubclassOf<class AACTR_BLOCKCORE> OakWood;
	UPROPERTY(EditDefaultsOnly, category = "Actors|Blocks|T0") TSubclassOf<class AACTR_BLOCKCORE> Iron;
	UPROPERTY(EditDefaultsOnly, category = "Actors|Blocks|T0") TSubclassOf<class AACTR_BLOCKCORE> Gold;



#pragma endregion Blocks

#pragma region Items

public:

	UPROPERTY(EditDefaultsOnly, category = "Actors|ItemsTextures") UTexture2D* TBase;
	UPROPERTY(EditDefaultsOnly, category = "Actors|ItemsTextures") UTexture2D* TStone;

#pragma endregion Items
	
};
