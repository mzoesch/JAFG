// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include <sqlite/sqlite3.h>


class JAFG_API DataAccessLayer {

public:

	DataAccessLayer(FString Path);
	~DataAccessLayer();

private:

	sqlite3* DB;
	static int callback(void* NotUsed, int argc, char** argv, char** azColName);

public:

	void TempExecute();
	
};
