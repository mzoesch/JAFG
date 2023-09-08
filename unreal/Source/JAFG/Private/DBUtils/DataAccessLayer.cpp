// © 2023 mzoesch. All rights reserved.

#include "DBUtils/DataAccessLayer.h"

#include "Misc/FileHelper.h"
#include "DBUtils/DBCORE.h"

DataAccessLayer::DataAccessLayer(const FString LvlName) {
	UE_LOG(LogTemp, Warning, TEXT("Initialize DAL for %s"), *LvlName)

	this->LvlName = LvlName;
	this->LvlPath = DBCORE::LevelFilesDirectory + LvlName;
	this->DBPath = this->LvlPath + TEXT("/") + DBCORE::LevelDBFile;
	
	if (!FPaths::FileExists(this->DBPath)) {
		UE_LOG(LogTemp, Warning, TEXT("LVL_DB could not be found, creating new one."))
		FFileHelper::SaveStringToFile(
			TEXT(""),
			*this->DBPath,
			FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM
		);
	}

	this->DB = nullptr;
	this->RC = sqlite3_open(TCHAR_TO_ANSI(*this->DBPath), &this->DB);

	if (this->RC != SQLITE_OK) {
		const FString Err = ANSI_TO_TCHAR(sqlite3_errmsg(this->DB));
		UE_LOG(LogTemp, Error, TEXT("Could not open LVL_DB."))
		UE_LOG(LogTemp, Error, TEXT("SQL error: %s"), *Err)
		sqlite3_close(this->DB);
		this->FatalError = 1;
		return;
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Opened LVL_DB successfully."))
	
	return;
	
	// const char* CPath = TCHAR_TO_ANSI(*LvlName);
	//
	// int exit = 0;
	// exit = sqlite3_open(cpath, &this->DB);
	// if (exit != SQLITE_OK) {
	// 	UE_LOG(LogTemp, Error, TEXT("Could not open DB"))
	// 	sqlite3_close(this->DB);
	// 	return;
	// }
	// else
	// 	UE_LOG(LogTemp, Warning, TEXT("Opened DB successfully"))
	//
	// FString sql = "SELECT * FROM GRADES;";
	// exit = 0;
	// exit = sqlite3_exec(this->DB, TCHAR_TO_ANSI(*sql), DataAccessLayer::Callback, 0, NULL);
	// if (exit != SQLITE_OK) {
	// 	UE_LOG(LogTemp, Error, TEXT("Could not execute query"))
	// 	sqlite3_close(this->DB);
	// 	return;
	// }
	// else
	// 	UE_LOG(LogTemp, Warning, TEXT("Executed query successfully"))
	//
	//
	//
	// sqlite3_close(this->DB);
	//
	// return;
}

DataAccessLayer::~DataAccessLayer() {
	UE_LOG(LogTemp, Warning, TEXT("Destroy DAL."))

	sqlite3_close(this->DB);
	UE_LOG(LogTemp, Warning, TEXT("Successfully cloesed LVL_DB."))
	
	return;
}

int DataAccessLayer::Callback(void* NotUsed, int argc, char** argv, char** azColName) {
	// for (int i = 0; i < argc; i++) {
	// 	
	// 	// This step is necessary to prevent UTF-8 character swapping
	// 	FString FazColName = azColName[i];
	// 	FString Fargv = argv[i];
	//
	// 	UE_LOG(LogTemp, Warning,
	// 		TEXT("[%d] = %s : %s"),
	// 		i,
	// 		*FazColName,
	// 		*Fargv
	// 	)
	// 	
	// 	continue;
	// }

	return 0;
}

void DataAccessLayer::TempExecute() {
	UE_LOG(LogTemp, Warning, TEXT("Executing TEMP_QUERY in DAL"))
	
	return;
}
