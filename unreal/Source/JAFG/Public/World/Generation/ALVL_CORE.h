// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/Generation/FChunk.h"

#include "DBUtils/DataAccessLayer.h"
#include "sqlite/sqlite3.h"

// #include "ALVL_CORE.generated.h"

class JAFG_API ALVL_CORE {
public:	
	
	ALVL_CORE();
	~ALVL_CORE();
	
private:

	DataAccessLayer* DAL;
	
	const char *CPath = "E:\\dev\\ue\\prjs\\JAFG\\unreal\\Content\\Data\\test5.db";
	sqlite3* DB;
	char* ZErrMsg = nullptr;
	int RC = 0;
	
public:

	TArray<FChunk> LoadedChunks;

public:

	// void UpdateLoadedChunks(bool bForceUpdate);
	// static const inline float WAIT_FOR_NEXT_CHUNK_UPDATE = 0.01f;
	// float TimeSinceLastChunkUpdate = 0.0f;
	//
	// void SV_LoadChunkToWorld(const FChunk& Chunk);
	// void BalanceLoadedChunks();

// Temp
public:

	// UPROPERTY(EditDefaultsOnly)
	// TSubclassOf<class AActor> TempChunkActorClass;

};
