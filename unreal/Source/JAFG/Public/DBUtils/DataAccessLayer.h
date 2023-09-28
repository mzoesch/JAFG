// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include <sqlite/sqlite3.h>

#include "World/Generation/FChunk2D.h"

#include "World/FJAFGCoordinateSystem.h"


class JAFG_API DataAccessLayer {

public:

	DataAccessLayer(const FString LvlName);
	~DataAccessLayer();

private:

	FString LvlName;
	FString LvlPath;
	FString DBPath;

public:

	static constexpr int CALLBACK_ERROR{-1};
	static constexpr int CALLBACK_BLOCK_AIR{-2};
	static constexpr int CALLBACK_CHUNK_GENERATED{-4};
	static constexpr int CALLBACK_CHUNK_NOT_GENERATED{-3};
	
private:
	
	int FatalError = 0;
	sqlite3* DB;
	char* ZErrMsg = nullptr;
	static int Callback(void* NotUsed, int argc, char** argv, char** azColName);

private:

	bool DefaultInit();
	bool DefaultInitCore();
	bool DefaultInitChunk();
	
public:
	
	int CheckIfChunkIsGenerated(const FChunk2D& Chunk2D);
	int LoadBlockIDFromDB(const FJAFGCoordinateSystem& BlockPosition);
	bool SaveChunkToGeneratedChunks(const FChunk2D& Chunk2D);
	bool SaveBlockIDToDB(const FJAFGCoordinateSystem& BlockPosition, const int BlockID);
	
};
