// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "FJAFGCoordinateSystem.h"
#include "Generation/FChunk.h"


class AACTR_BLOCKCORE;


static class JAFG_API WorldManipulation {
public:
	
	static TSubclassOf<AACTR_BLOCKCORE> GetBlockClass(UWorld* World, const FString& BlockID);
	static TSubclassOf<AACTR_BLOCKCORE> GetBlockClass_V2(const UWorld* World, const int BlockID);
	static void SpawnBlock(
		UWorld* World,
		const FJAFGCoordinateSystem& BlockPosition,
		const FString& BlockID
	);	
	static void SpawnBlock_V2(
		UWorld* World,
		const FJAFGCoordinateSystem& BlockPosition,
		const int BlockID
	);

};
