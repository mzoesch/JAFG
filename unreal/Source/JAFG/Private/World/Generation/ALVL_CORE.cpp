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

		if (this->Chunk2DsLoaded.Contains(Chunk) || this->Chunk2DsQueue.Contains(Chunk))
			continue;

		UE_LOG(LogTemp, Warning, TEXT("Adding new chunk %s"), *Chunk.ToString())
		this->Chunk2DsQueue.Add(Chunk);
		continue;
	}

	this->TransformChunk2DsTo3Ds();
	this->LoadPrepared3DChunks();
	
	return;
}

void ALVL_CORE::TransformChunk2DsTo3Ds() {
	if (this->Chunk2DsQueue.Num() > 0) {
		const int IsGenerated = this->DAL->CheckIfChunkIsGenerated(this->Chunk2DsQueue[0]);

		if (IsGenerated == DataAccessLayer::CALLBACK_CHUNK_GENERATED) {
			this->PrepareChunk2DToLoad(this->Chunk2DsQueue[0]);
			this->Chunk2DsLoaded.Add(this->Chunk2DsQueue[0]);
			this->Chunk2DsQueue.RemoveAt(0);
			return;
		}

		if (IsGenerated == DataAccessLayer::CALLBACK_CHUNK_NOT_GENERATED) {
			this->GenerateFChunk(this->Chunk2DsQueue[0]);
			return;
		}
		
		if (IsGenerated == DataAccessLayer::CALLBACK_ERROR) {
			UE_LOG(LogTemp, Error, TEXT("ERROR: Could not check if chunk is generated (%s)."), *this->Chunk2DsQueue[0].ToString())
			this->Chunk2DsQueue.RemoveAt(0);
			return;
		}
		
		return;
	}

	return;
}

void ALVL_CORE::LoadPrepared3DChunks() {
	// Only loading one sub-chunk
	if (this->ChunksQueue.Num() > 0) {
		this->LoadChunkToWorld(this->ChunksQueue[0]);
		this->ChunksQueue.RemoveAt(0);
		
		return;
	}
	
	return;
}

void ALVL_CORE::PrepareChunk2DToLoad(const FChunk2D& Chunk2D) {
	for (int i = 0; i < FChunk2D::VERTICAL_SUB_CHUNKS; i++) {
		FChunk Chunk = Chunk2D.AllSubChunks()[i];
		this->ChunksQueue.Add(Chunk);
		
		continue;
	}

	return;
}

void ALVL_CORE::LoadChunkToWorld(const FChunk& Chunk) const {
	for (const FJAFGCoordinateSystem& BlockPosition : Chunk.AllBlockPositions()) {
		this->LoadBlockToWorld(BlockPosition);
		continue;
	}

	return;
}

void ALVL_CORE::LoadBlockToWorld(const FJAFGCoordinateSystem& BlockPosition) const {
	const int BlockID = this->DAL->LoadBlockIDFromDB(BlockPosition);

	if (BlockID == Blocks::ID_Air)
		return;
		
	if (BlockID == DataAccessLayer::CALLBACK_ERROR) {
		UE_LOG(LogTemp, Error, TEXT("FATAL ERROR: Could not load block from db (%s)."), *BlockPosition.ToString())
		return;
	}
	
	WorldManipulation::SpawnBlock_V2(
		this->GI->GetWorld(),
		BlockPosition,
		BlockID
	);
	
	return;
}

void ALVL_CORE::GenerateFChunk(const FChunk2D& Chunk2D) const {
	const bool bSuccess = this->DAL->SaveChunkToGeneratedChunks(Chunk2D);
	if (bSuccess == false) {
		UE_LOG(LogTemp, Error, TEXT("ERROR: Could not save chunk to db (%s)."), *Chunk2D.ToString())
		return;
	}
	
	for (const FChunk& Chunk : Chunk2D.AllSubChunks()) {
		for (const FJAFGCoordinateSystem& BlockPosition : Chunk.AllBlockPositions()) {
			if (BlockPosition.Z == "0" || BlockPosition.Z == "28") {
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
