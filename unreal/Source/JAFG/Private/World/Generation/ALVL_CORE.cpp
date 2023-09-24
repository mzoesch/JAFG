// © 2023 mzoesch. All rights reserved.


#include "World/Generation/ALVL_CORE.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h" 

#include "CORE/UGI_CORE.h"
#include "World/Blocks.h"
#include "UObject/FastReferenceCollector.h"
#include "World/WorldManipulation.h"

ALVL_CORE::ALVL_CORE() {
	UE_LOG(LogTemp, Error, TEXT("Initialize ALVL_CORE without sql db."))
	
	return;
}

ALVL_CORE::ALVL_CORE(UGI_CORE* GI) {
	UE_LOG(LogTemp, Warning, TEXT("Initialize ALVL_CORE with world name %s."), *GI->GetSessionName())
	
	this->GI = GI;
	this->DAL = new DataAccessLayer(
		this->GI->GetSessionName()
	);
	
	return;
}

ALVL_CORE::~ALVL_CORE() {
	UE_LOG(LogTemp, Warning, TEXT("Destroy ALVL_CORE."))

	delete this->DAL;

	return;
}

void ALVL_CORE::UpdateLoadedChunksFromTransformArray(const TArray<FTransform>& HotTransforms) {
	for (const FTransform& HotTransform : HotTransforms) {
		FChunk Chunk = FChunk::FromWorldPosition(HotTransform.GetLocation());
		if (this->LoadedChunks.Contains(Chunk))
			continue;
		this->LoadedChunks.Add(Chunk);
		UE_LOG(LogTemp, Warning, TEXT("Added chunk %s."), *Chunk.ToString())
		
		this->LoadChunkToWorld(Chunk);

		continue;
	}

	for (const FChunk& Chunk : this->LoadedChunks) {
		UE_LOG(LogTemp, Warning, TEXT("CURRENTLY LOADED : %s"), *Chunk.ToString())
		continue;
	}
	
	return;
}

void ALVL_CORE::LoadChunkToWorld(const FChunk& Chunk) const {
	bool bFirst = true;
	for (const FJAFGCoordinateSystem& BlockPosition : Chunk.AllBlockPositions()) {
		if (bFirst) {
			this->GenerateFChunk(Chunk);
			bFirst = false;
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *BlockPosition.ToString())
		this->LoadBlockToWorld(BlockPosition);
		continue;
	}
	
	return;
}

void ALVL_CORE::LoadBlockToWorld(const FJAFGCoordinateSystem& BlockPosition) const {
	const int BlockID = this->DAL->LoadBlockIDFromDB(BlockPosition);

	if (BlockID == Blocks::ID_Air)
		return;
	
	if (BlockID == -1) {
		UE_LOG(LogTemp, Error, TEXT("ERROR: Could not load block from db (%s)."), *BlockPosition.ToString())
		return;
	}

	if (BlockID == -2) {
		UE_LOG(LogTemp, Warning, TEXT("Block does not exist in db (%s)."), *BlockPosition.ToString())
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("BlockID: %d"), BlockID)
	WorldManipulation::SpawnBlock_V2(
		this->GI->GetWorld(),
		BlockPosition,
		BlockID
	);
	
	return;
}

void ALVL_CORE::GenerateFChunk(const FChunk& Chunk) const {
	UE_LOG(LogTemp, Warning, TEXT("Generating FChunk %s."), *Chunk.ToString())
	
	for (const FJAFGCoordinateSystem& BlockPosition : Chunk.AllBlockPositions()) {
		if (BlockPosition.Z == "0" || BlockPosition.Z == "-0") {
			UE_LOG(LogTemp, Warning, TEXT("Generating block base (%s)."), *BlockPosition.ToString())

			if (this->DAL->SaveBlockIDToDB(BlockPosition, Blocks::ID_Base) == false) {
				UE_LOG(LogTemp, Error, TEXT("ERROR: Could not save block to db (%s)."), *BlockPosition.ToString())
				continue;
			}

			continue;
		}

		if (this->DAL->SaveBlockIDToDB(BlockPosition, Blocks::ID_Air) == false) {
			UE_LOG(LogTemp, Error, TEXT("ERROR: Could not save block to db (%s)."), *BlockPosition.ToString())
			continue;
		}
		
		continue;
	}
	
	return;
}
