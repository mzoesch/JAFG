// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "World/Generation/ALVL_CORE.h"
#include <sqlite/sqlite3.h>

#include "AGM_CORE.generated.h"


class ALVL_CORE;


UCLASS()
class JAFG_API AGM_CORE : public AGameMode {
	GENERATED_BODY()

public:

	AGM_CORE();
	~AGM_CORE();

public:

	virtual void Tick(float DeltaSeconds) override;
	
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

#pragma region World

public:

	// TODO: Make a list with all AActors that will need a InGame Tick Event
	// Let them register to this list in their BeginPlay() and unregister in their EndPlay()

private:

	ALVL_CORE* LvlCore = nullptr;

private:

	static const inline float WAIT_FOR_NEXT_CHUNK_UPDATE = 2.5f;
	float TimeSinceLastChunkUpdate = AGM_CORE::WAIT_FOR_NEXT_CHUNK_UPDATE;
	void UpdateLoadedChunks(bool bForceUpdate);
	
#pragma endregion World
	
};
