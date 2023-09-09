// © 2023 mzoesch. All rights reserved.


#include "CORE/AGM_CORE.h"

#include "Kismet/GameplayStatics.h"

#include "CORE/UGI_CORE.h"
#include "World/Generation/ALVL_CORE.h"

AGM_CORE::AGM_CORE() {
	UE_LOG(LogTemp, Warning, TEXT("Initialize AGM_CORE"))

	UGI_CORE* GI = Cast<UGI_CORE>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GI) {
		UE_LOG(LogTemp, Error, TEXT("Could not cast UGameInstance to UGI_CORE."))
		return;
	}
	check(GI)
	this->LvlCore = new ALVL_CORE(GI);
	
	return;
}

AGM_CORE::~AGM_CORE() {
	UE_LOG(LogTemp, Warning, TEXT("Destroy AGM_CORE"))

	delete this->LvlCore;
	
	return;
}

void AGM_CORE::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	this->UpdateLoadedChunks(false);
	
	return;
}

#pragma region World

void AGM_CORE::UpdateLoadedChunks(bool bForceUpdate) {

	if (!bForceUpdate) {
		this->TimeSinceLastChunkUpdate += this->GetWorld()->GetDeltaSeconds();
		if (this->TimeSinceLastChunkUpdate < this->WAIT_FOR_NEXT_CHUNK_UPDATE)
			return;
	}
	this->TimeSinceLastChunkUpdate = 0.0f;
	
	UE_LOG(LogTemp, Warning, TEXT("Updating loaded chunks"))
	
	return;
}

#pragma endregion World

// Make a tick funktion. With ingame ticks (timeline).
// And call all other tick functions from here.
