// © 2023 mzoesch. All rights reserved.

#include "DBUtils/DataAccessLayer.h"

#include "Misc/FileHelper.h"
#include "DBUtils/DBCORE.h"
#include "World/Blocks.h"
#include "UObject/FastReferenceCollector.h"

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
	const int RC = sqlite3_open(TCHAR_TO_ANSI(*this->DBPath), &this->DB);

	if (RC != SQLITE_OK) {
		const FString Err = ANSI_TO_TCHAR(sqlite3_errmsg(this->DB));
		UE_LOG(LogTemp, Error, TEXT("Could not open LVL_DB."))
		UE_LOG(LogTemp, Error, TEXT("SQL error: %s"), *Err)
		sqlite3_close(this->DB);
		this->FatalError = 1;
		return;
	}

	if (!this->DefaultInit()) {
		const FString Err = "Failed to initialize LVL_DB.";
		UE_LOG(LogTemp, Error, TEXT("%s"), *Err)
		sqlite3_close(this->DB);
		this->FatalError = 1;
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Opened LVL_DB successfully."))
	
	return;
}

DataAccessLayer::~DataAccessLayer() {
	UE_LOG(LogTemp, Warning, TEXT("Destroy DAL."))

	sqlite3_close(this->DB);
	UE_LOG(LogTemp, Warning, TEXT("Successfully cloesed LVL_DB."))
	
	return;
}

int DataAccessLayer::Callback(void* NotUsed, int argc, char** argv, char** azColName) {
	return 0;
}

bool DataAccessLayer::DefaultInit() {
	FString StructuredQueryLanguage = FString::Printf(
		TEXT("CREATE TABLE IF NOT EXISTS %s (")
		TEXT("BlockID TINYINT NOT NULL,")
		TEXT("%s TEXT NOT NULL,")
		TEXT("%s TEXT NOT NULL,")
		TEXT("%s TEXT NOT NULL,")
		TEXT("PRIMARY KEY (%s, %s, %s)")
		TEXT(");"),
		*DBCORE::InnerDBCoreTableName,
		*DBCORE::InnerDBCorePrimaryKeyXColumnName,
		*DBCORE::InnerDBCorePrimaryKeyYColumnName,
		*DBCORE::InnerDBCorePrimaryKeyZColumnName,
		*DBCORE::InnerDBCorePrimaryKeyXColumnName,
		*DBCORE::InnerDBCorePrimaryKeyYColumnName,
		*DBCORE::InnerDBCorePrimaryKeyZColumnName
	);

	UE_LOG(LogTemp, Warning, TEXT("SQL Exec: %s"), *StructuredQueryLanguage)

	const int Exit = sqlite3_exec(
		this->DB,
		TCHAR_TO_ANSI(*StructuredQueryLanguage),
		this->Callback,
		nullptr,
		&this->ZErrMsg
	);

	if (Exit != SQLITE_OK) {
		const FString Err = ANSI_TO_TCHAR(this->ZErrMsg);
		UE_LOG(LogTemp, Error, TEXT("SQL Error: %s"), *Err)
		sqlite3_free(this->ZErrMsg);
		return false;
	}
	
	return true;
}

int DataAccessLayer::LoadBlockIDFromDB(
		const FJAFGCoordinateSystem& BlockPosition
	) const {
	const FString StructuredQueryLanguage = FString::Printf(
		TEXT("SELECT %s FROM %s WHERE %s = %s AND %s = %s AND %s = %s LIMIT 1;"),
		*DBCORE::InnerDBCoreBlockIDColumnName,
		*DBCORE::InnerDBCoreTableName,
		*DBCORE::InnerDBCorePrimaryKeyXColumnName,
		*BlockPosition.X,
		*DBCORE::InnerDBCorePrimaryKeyYColumnName,
		*BlockPosition.Y,
		*DBCORE::InnerDBCorePrimaryKeyZColumnName,
		*BlockPosition.Z
	);
	
	sqlite3_stmt *stmt;
	int RC = sqlite3_prepare_v2(
		this->DB,
		TCHAR_TO_ANSI(*StructuredQueryLanguage),
		-1,
		&stmt,
		nullptr
	);

	if (RC != SQLITE_OK) {
		const FString Err = ANSI_TO_TCHAR(sqlite3_errmsg(this->DB));
		UE_LOG(LogTemp, Error, TEXT("SQL error: %s"), *Err)
		sqlite3_close(this->DB);
		return -1;
	}

	if ((RC = sqlite3_step(stmt)) == SQLITE_ROW) {
		const int ID = sqlite3_column_int(stmt, 0); 
		sqlite3_finalize(stmt);
		return ID;
	}
	
	sqlite3_finalize(stmt);
	
	return -2;
}

bool DataAccessLayer::SaveBlockIDToDB(
	const FJAFGCoordinateSystem& BlockPosition,
	const int BlockID
	) const {

	// ignore air for now
	if (BlockID == Blocks::ID_Air)
	{
		return true;
	}
	
	
	const FString StructuredQueryLanguage = FString::Printf(
		TEXT("INSERT INTO %s (%s, %s, %s, %s) VALUES (%d, \"%s\", \"%s\", \"%s\");"),
		*DBCORE::InnerDBCoreTableName,
		*DBCORE::InnerDBCoreBlockIDColumnName,
		*DBCORE::InnerDBCorePrimaryKeyXColumnName,
		*DBCORE::InnerDBCorePrimaryKeyYColumnName,
		*DBCORE::InnerDBCorePrimaryKeyZColumnName,
		BlockID,
		*BlockPosition.X,
		*BlockPosition.Y,
		*BlockPosition.Z
	);

	UE_LOG(LogTemp, Warning, TEXT("SQL Exec: %s"), *StructuredQueryLanguage)

	char* LocalZErrMsg = nullptr;
	const int Exit = sqlite3_exec(
		this->DB,
		TCHAR_TO_ANSI(*StructuredQueryLanguage),
		this->Callback,
		nullptr,
		&LocalZErrMsg
	);

	if (Exit != SQLITE_OK) {
		const FString Err = ANSI_TO_TCHAR(LocalZErrMsg);
		UE_LOG(LogTemp, Error, TEXT("SQL Error: %s"), *Err)
		sqlite3_free(LocalZErrMsg);
		return false;
	}
	
	return true;
}

// Create new table with all generated chunks in db
// So we can check if a block is air or not generated yet 
