// © 2023 mzoesch. All rights reserved.


#include "World/Generation/ALVL_CORE.h"

#include "Kismet/GameplayStatics.h" 
#include "Engine/World.h" 

#include "CORE/UGI_CORE.h"

ALVL_CORE::ALVL_CORE() {
	UE_LOG(LogTemp, Warning, TEXT("Initialize ALVL_CORE without sql db."))
	
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


// void ALVL_CORE::UpdateLoadedChunks(
// 	bool bForceUpdate
// ) {
// 	if (!bForceUpdate) {	
// 		float DeltaTime = this->GetWorld()->GetDeltaSeconds();
// 		this->TimeSinceLastChunkUpdate += DeltaTime;
// 		if (this->TimeSinceLastChunkUpdate < this->WAIT_FOR_NEXT_CHUNK_UPDATE)
// 			return;
// 		this->TimeSinceLastChunkUpdate = 0.0f;
// 	}
// 	
// 	TArray<AActor*> FoundActors;
// 	TArray<FTransform> PlayerTransforms;
// 	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), FoundActors);
// 	for (AActor* Actor : FoundActors)
// 		PlayerTransforms.Add(Cast<APC_CORE>(Actor)->GetPlayerTransform());
//
// 	for (int i = 0; i < PlayerTransforms.Num(); i++) {
// 		FChunk HighValueChunk = FChunk::FromWorldPosition(PlayerTransforms[i].GetLocation());
//
// 		bool bHighValueChunkIsLoaded = false;
// 		for (int j = 0; j < this->LoadedChunks.Num(); j++) {
// 			if (HighValueChunk == this->LoadedChunks[j]) {
// 				bHighValueChunkIsLoaded = true;
// 				break;
// 			}
// 			continue;
// 		}
//
// 		if (!bHighValueChunkIsLoaded)
// 			this->LoadedChunks.Add(HighValueChunk);
//
// 		continue;
// 	}
//
// 	this->BalanceLoadedChunks();
// 	return;
// }
//
// void ALVL_CORE::BalanceLoadedChunks() {
// 	for (int i = 0; i < this->LoadedChunks.Num(); i++) {
// 		if (this->LoadedChunks[i].bIsLoaded == true)
// 			continue;
// 		
// 		this->SV_LoadChunkToWorld(this->LoadedChunks[i]);
//
// 		//AActor* Chunk = this->GetWorld()->SpawnActor<AActor>(
// 		//	this->TempChunkActorClass,
// 		//	this->LoadedChunks[i].GetMiddleAsTransform()
// 		//);
// 		//Chunk->SetReplicates(true);
// 		
// 		this->LoadedChunks[i].bIsLoaded = true;
// 		continue;
// 	}
//
// 	return;
// }
//
// void ALVL_CORE::SV_LoadChunkToWorld_Implementation(const FChunk& Chunk) {
// 	WorldManipulation::LoadChunk(
// 		this->GetWorld(),
// 		Chunk
// 	);
//
// 	return;
// }
