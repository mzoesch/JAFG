// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DBUtils/DataAccessLayer.h"

#include "World/Generation/FChunk.h"

class UGI_CORE;


class JAFG_API ALVL_CORE {
public:	
	
	ALVL_CORE();
	ALVL_CORE(UGI_CORE* GI);
	~ALVL_CORE();
	
private:
	
	UGI_CORE* GI;

private:

	DataAccessLayer* DAL;

public:

	void UpdateLoadedChunksFromTransformArray(const TArray<FTransform>& HotTransforms);

private:

	TArray<FChunk> LoadedChunks;

	void LoadChunkToWorld(const FChunk& Chunk) const;
	void LoadBlockToWorld(const FJAFGCoordinateSystem& BlockPosition, const bool bAbortAfterFailure = false) const;

	void GenerateFChunk(const FChunk& Chunk) const;
	
};
