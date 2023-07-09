// © 2023 mzoesch. All rights reserved.


#include "World/Generation/ALVL_CORE.h"

#include "Kismet/GameplayStatics.h"
#include "CORE/APC_CORE.h"

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

TArray<FTransform> ALVL_CORE::UpdateLoadedChunks() {

	TArray<AActor*> FoundActors;
	TArray<FTransform> PlayerTransforms;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
		PlayerTransforms.Add(Cast<APC_CORE>(Actor)->GetPlayerTransform());

	return PlayerTransforms;
}
