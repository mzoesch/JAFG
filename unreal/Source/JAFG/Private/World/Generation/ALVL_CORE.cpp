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
		FChunk2D Chunk = FChunk2D::FromWorldPosition(HotTransform.GetLocation());
		if (this->LoadedChunks.Contains(Chunk))
			continue;
		
		UE_LOG(LogTemp, Warning, TEXT("Loading Chunk %s."), *Chunk.ToString())
		this->PrepareChunk2DToLoad(Chunk);
		this->LoadedChunks.Add(Chunk);
		
		continue;
	}

	for (const FChunk2D& Chunk : this->LoadedChunks) {
		UE_LOG(LogTemp, Warning, TEXT("CURRENTLY LOADED : %s"), *Chunk.ToString())
		continue;
	}

	// Only loading one subchunk at a time for now
	for (const FChunk& Chunk : this->ChunkQueue) {
		UE_LOG(LogTemp, Warning, TEXT("CURRENTLY QUEUED : %s -> Loading . . ."), *Chunk.ToString())
		this->LoadChunkToWorld(Chunk);
		break;
	}

	if (this->ChunkQueue.Num() > 0)
		this->ChunkQueue.RemoveAt(0);
	
	return;
}

void ALVL_CORE::PrepareChunk2DToLoad(const FChunk2D& Chunk2D) {
	for (int i = 0; i < FChunk2D::VERTICAL_SUB_CHUNKS; i++) {
		FChunk Chunk = Chunk2D.AllSubChunks()[i];
		this->ChunkQueue.Add(Chunk);
		
		continue;
	}

	return;
}

void ALVL_CORE::LoadChunkToWorld(const FChunk& Chunk) const {
	for (const FJAFGCoordinateSystem& BlockPosition : Chunk.AllBlockPositions()) {
		UE_LOG(LogTemp, Warning, TEXT("Loading Block %s"), *BlockPosition.ToString())
		this->LoadBlockToWorld(BlockPosition);
		continue;
	}

	return;
}

void ALVL_CORE::LoadBlockToWorld(const FJAFGCoordinateSystem& BlockPosition) const {
	const int BlockID = this->DAL->LoadBlockIDFromDB(BlockPosition);

	if (BlockID == Blocks::ID_Air)
		return;
	
	if (BlockID == DataAccessLayer::CALLBACK_CHUNK_NOT_GENERATED) {
		UE_LOG(LogTemp, Warning,
			TEXT("Chunk %s not generated yet. Generating . . ."),
			*FChunk2D::FromWorldPosition(BlockPosition.GetAsUnrealVector()).ToString()
		)

		this->GenerateFChunk(FChunk2D::FromWorldPosition(BlockPosition.GetAsUnrealVector()));
		this->LoadBlockToWorld(BlockPosition);
		
		return;
	}
		
	if (BlockID == DataAccessLayer::CALLBACK_ERROR) {
		UE_LOG(LogTemp, Error, TEXT("FATAL ERROR: Could not load block from db (%s)."), *BlockPosition.ToString())
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Spawing BlockID: %d"), BlockID)
	WorldManipulation::SpawnBlock_V2(
		this->GI->GetWorld(),
		BlockPosition,
		BlockID
	);
	
	return;
}

void ALVL_CORE::GenerateFChunk(const FChunk2D& Chunk2D) const {
	UE_LOG(LogTemp, Warning, TEXT("Generating FChunk %s."), *Chunk2D.ToString())

	const bool bSuccess = this->DAL->SaveChunkToGeneratedChunks(Chunk2D);
	if (bSuccess == false) {
		UE_LOG(LogTemp, Error, TEXT("ERROR: Could not save chunk to db (%s)."), *Chunk2D.ToString())
		return;
	}
	
	for (const FChunk& Chunk : Chunk2D.AllSubChunks()) {
		for (const FJAFGCoordinateSystem& BlockPosition : Chunk.AllBlockPositions()) {
			if (BlockPosition.Z == "0" || BlockPosition.Z == "28") {
				UE_LOG(LogTemp, Warning, TEXT("Generating block base (%s)."), *BlockPosition.ToString())

				if (this->DAL->SaveBlockIDToDB(BlockPosition, Blocks::ID_Base) == false) {
					UE_LOG(LogTemp, Error, TEXT("ERROR: Could not save block to db (%s)."), *BlockPosition.ToString())
					continue;
				}

				continue;
			}
			
			continue;
		}
		
		continue;
	}
	
	return;
}
