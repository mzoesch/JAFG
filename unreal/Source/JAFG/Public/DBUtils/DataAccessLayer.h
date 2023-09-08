// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include <sqlite/sqlite3.h>


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
	int RC = 0;
	char* ZErrMsg = nullptr;
	static int Callback(void* NotUsed, int argc, char** argv, char** azColName);

public:

	void TempExecute();
	
};
