// © 2023 mzoesch. All rights reserved.


#include "BPUtils/BFL_Generic.h"

#include "DBUtils/DBCORE.h"

FString UBFL_Generic::GetFullLevelSavePathFromLevelName(const FString LevelName) {
	return DBCORE::GetFullPathFromLevelName(&LevelName);
}
