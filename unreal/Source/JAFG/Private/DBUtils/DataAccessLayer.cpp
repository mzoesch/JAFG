// © 2023 mzoesch. All rights reserved.


#include "DBUtils/DataAccessLayer.h"


DataAccessLayer::DataAccessLayer(FString Path) {
	UE_LOG(LogTemp, Warning, TEXT("Create DAL"))

	const char* cpath = "";
	cpath = TCHAR_TO_ANSI(*Path);

	int exit = 0;
	exit = sqlite3_open(cpath, &this->DB);
	if (exit != SQLITE_OK) {
		UE_LOG(LogTemp, Error, TEXT("Could not open DB"))
		sqlite3_close(this->DB);
		return;
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Opened DB successfully"))

	FString sql = "SELECT * FROM GRADES;";
	exit = 0;
	exit = sqlite3_exec(this->DB, TCHAR_TO_ANSI(*sql), callback, 0, NULL);
	if (exit != SQLITE_OK) {
		UE_LOG(LogTemp, Error, TEXT("Could not execute query"))
		sqlite3_close(this->DB);
		return;
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Executed query successfully"))



	sqlite3_close(this->DB);

	return;
}

DataAccessLayer::~DataAccessLayer() {
	UE_LOG(LogTemp, Warning, TEXT("Destroy DAL"))

	return;
}

int DataAccessLayer::callback(void* NotUsed, int argc, char** argv, char** azColName) {
	UE_LOG(LogTemp, Warning,TEXT("Callback"))

	for (int i = 0; i < argc; i++) {
		FString FIazColName = azColName[i];
		FString FIargv = argv[i];

		UE_LOG(LogTemp, Warning,
			TEXT("[%d] = %s : %s"),
			i,
			*FIazColName,
			*FIargv
		)
		continue;
	}

	return 0;
}

void DataAccessLayer::TempExecute() {
	UE_LOG(LogTemp, Warning, TEXT("Executing TEMP_QUERY in DAL"))
	
	return;
}
