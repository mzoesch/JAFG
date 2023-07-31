// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"


static class JAFG_API DBCORE {

public:

	static const inline FString DataBaseFolder = FPaths::ProjectContentDir() + TEXT("Data/");

	static inline int Runs = 0;
	
public:

	static void TEMP_QUERY();
	
};
