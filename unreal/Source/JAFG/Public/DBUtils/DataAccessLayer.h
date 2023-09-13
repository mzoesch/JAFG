// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include <sqlite/sqlite3.h>

#include "World/FJAFGCoordinateSystem.h"


class JAFG_API DataAccessLayer {

public:

	DataAccessLayer(const FString LvlName);
	~DataAccessLayer();

private:

	FString LvlName;
	FString LvlPath;
	FString DBPath;
	
private:

	int FatalError = 0;
	sqlite3* DB;
	char* ZErrMsg = nullptr;
	static int Callback(void* NotUsed, int argc, char** argv, char** azColName);

private:

	bool DefaultInit();
	
public:

	// -2: Block does not exist in DB; -1: Fatal Error; 0>=: BlockID
	int LoadBlockIDFromDB(const FJAFGCoordinateSystem& BlockPosition) const;
	bool SaveBlockIDToDB(const FJAFGCoordinateSystem& BlockPosition, const int BlockID) const;
	
};
