// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"


static class JAFG_API DBCORE {
public:
	
	static const inline FString LevelDBFile = TEXT("db.db");
	static const inline FString GameFilesDirectory = FPaths::ProjectContentDir() + TEXT("Data/");
	static const inline FString LevelFilesDirectory = FPaths::ProjectContentDir() + TEXT("Data/") + TEXT("Levels/");

	static inline FString GetFullPathFromLevelName(const FString* LevelName) {
		return FPaths::ConvertRelativePathToFull(DBCORE::LevelFilesDirectory + *LevelName);
	}
	
};
