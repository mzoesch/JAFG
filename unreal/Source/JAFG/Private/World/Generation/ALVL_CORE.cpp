// © 2023 mzoesch. All rights reserved.


#include "World/Generation/ALVL_CORE.h"

#include "Kismet/GameplayStatics.h"
#include "CORE/APC_CORE.h"
#include "CORE/AGM_CORE.h"

ALVL_CORE::ALVL_CORE() {
 	
	// Set this actor to call Tick() every frame.
	// You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	return;
}

void ALVL_CORE::BeginPlay() {
	Super::BeginPlay();

	return;
}

void ALVL_CORE::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	return;
}

void ALVL_CORE::UpdateLoadedChunks(
	bool bForceUpdate
) {
	if (!bForceUpdate) {	
		float DeltaTime = this->GetWorld()->GetDeltaSeconds();
		this->TimeSinceLastChunkUpdate += DeltaTime;
		if (this->TimeSinceLastChunkUpdate < this->WAIT_FOR_NEXT_CHUNK_UPDATE)
			return;
		this->TimeSinceLastChunkUpdate = 0.0f;
	}

	TArray<AActor*> FoundActors;
	TArray<FTransform> PlayerTransforms;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
		PlayerTransforms.Add(Cast<APC_CORE>(Actor)->GetPlayerTransform());

	for (int i = 0; i < PlayerTransforms.Num(); i++) {
		FChunk HighValueChunk = FChunk::FromWorldPosition(PlayerTransforms[i].GetLocation());

		bool bHighValueChunkIsLoaded = false;
		for (int j = 0; j < this->LoadedChunks.Num(); j++) {
			if (HighValueChunk == this->LoadedChunks[j]) {
				bHighValueChunkIsLoaded = true;
				break;
			}
			continue;
		}

		if (!bHighValueChunkIsLoaded)
			this->LoadedChunks.Add(HighValueChunk);

		continue;
	}

	this->BalanceLoadedChunks();
	return;
}

void ALVL_CORE::BalanceLoadedChunks() {
	for (int i = 0; i < this->LoadedChunks.Num(); i++) {
		if (this->LoadedChunks[i].bIsLoaded == true)
			continue;
		
		AGM_CORE* GM = Cast<AGM_CORE>(this->GetWorld()->GetAuthGameMode());
		check(GM);

		AActor* Chunk = this->GetWorld()->SpawnActor<AActor>(
			this->TempChunkActorClass,
			this->LoadedChunks[i].GetMiddleAsTransform()
		);
		Chunk->SetReplicates(true);
		this->LoadedChunks[i].bIsLoaded = true;
		
		continue;
	}

	return;
}
