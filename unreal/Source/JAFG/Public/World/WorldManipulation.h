// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FJAFGCoordinateSystem.h"


class AACTR_BLOCKCORE;


static class JAFG_API WorldManipulation {
public:
	
	static TSubclassOf<AACTR_BLOCKCORE> GetBlockClass(UWorld* World, const FString& BlockID);
	static void SpawnBlock(
		UWorld* World,
		const FJAFGCoordinateSystem& BlockPosition,
		const FString& BlockID
	);

};
