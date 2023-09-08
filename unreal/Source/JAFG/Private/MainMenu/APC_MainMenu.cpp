// © 2023 mzoesch. All rights reserved.


#include "MainMenu/APC_MainMenu.h"

#include "HAL/FileManagerGeneric.h"

#include "DBUtils/DBCORE.h"

TArray<FLocalSave> APC_MainMenu::GetAllLocalSaves() {
	const FString LocalSavesDirectory = DBCORE::LevelFilesDirectory;
	UE_LOG(LogTemp, Warning, TEXT("Searching for local saves in: %s"), *LocalSavesDirectory)

	TArray<FLocalSave> LocalSaves;
	TArray<FString> FoundDirectories;
	FFileManagerGeneric::Get().FindFilesRecursive(
		FoundDirectories,
		*LocalSavesDirectory,
		TEXT("*"),
		false,
		true,
		true
	);
	
	for (const FString Directory : FoundDirectories) {
		const FString DBFile = Directory + TEXT("/") + DBCORE::LevelDBFile;
		const FString LevelName = Directory.RightChop(Directory.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd) + 1);
		
		if (!FFileManagerGeneric::Get().FileExists(*DBFile)) {
			UE_LOG(LogTemp, Warning,
				TEXT("Found %s directory in Level Files Directory but could not find the appropiate level database."),
				*LevelName
			)
			
			continue;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Found saved local level: %s"), *LevelName)
		const FLocalSave LocalSave = FLocalSave(LevelName, TEXT("01/01/2021"));
		LocalSaves.Add(LocalSave);
		
		continue;
	}
	
	return LocalSaves;
}

int APC_MainMenu::DeleteLocalSave(const FString& SaveName) {
	UE_LOG(LogTemp, Warning, TEXT("Try deleting local save: %s"), *SaveName)
	
	const FString LocalSaveDirectory = DBCORE::LevelFilesDirectory + SaveName;
	if (!FFileManagerGeneric::Get().DirectoryExists(*LocalSaveDirectory)) {
		UE_LOG(LogTemp, Warning,
			TEXT("Could not find local save directory to delete: %s"),
			*LocalSaveDirectory
		)
		return 1;
	}
	
	FFileManagerGeneric::Get().DeleteDirectory(*LocalSaveDirectory, false, true);
	UE_LOG(LogTemp, Warning, TEXT("Deleted local save directory: %s"), *LocalSaveDirectory)
	
	return 0;
}
