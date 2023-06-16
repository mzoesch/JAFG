// © 2023 mzoesch. All rights reserved.


#include "CORE/AACTR_BLOCKCORE.h"

#include "CORE/AACTR_BLOCKCORE.h"

// Sets default values
AACTR_BLOCKCORE::AACTR_BLOCKCORE() {
 	// Set this actor to call Tick() every frame.
	// You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->StaticMeshComponent =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	this->RootComponent = this->StaticMeshComponent;

	return;
}

AACTR_BLOCKCORE::AACTR_BLOCKCORE(FString BlockID) {
	AACTR_BLOCKCORE();

	this->BlockID = BlockID;
	return;
}

// Called when the game starts or when spawned
void AACTR_BLOCKCORE::BeginPlay() {
	Super::BeginPlay();
	return;
}

// Called every frame
void AACTR_BLOCKCORE::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("WARNING: AACTR_BLOCKCORE should never be called."));
	return;
}

FVector AACTR_BLOCKCORE::GetBlockLocation() {
	return this->GetActorLocation();
}

FJAFGCoordinateSystem AACTR_BLOCKCORE::GetNewBlockPositionOnHit(FHitResult HitResult) {
	FVector Offset = HitResult.ImpactPoint - this->GetBlockLocation();
	FJAFGCoordinateSystem JAFGLocation = FJAFGCoordinateSystem(this->GetBlockLocation());

	if (FMath::IsNearlyEqual(Offset.X, 50.f, 0.01f)) {
		JAFGLocation.IncreaseToNorth();
	}
	if (FMath::IsNearlyEqual(Offset.X, -50.f, 0.01f)) {
		JAFGLocation.IncreaseToSouth();
	}

	if (FMath::IsNearlyEqual(Offset.Y, 50.f, 0.01f)) {
		JAFGLocation.IncreaseToEast();
	}
	if (FMath::IsNearlyEqual(Offset.Y, -50.f, 0.01f)) {
		JAFGLocation.IncreaseToWest();
	}

	if (FMath::IsNearlyEqual(Offset.Z, 50.f, 0.01f)) {
		JAFGLocation.IncreaseUpwards();
	}
	if (FMath::IsNearlyEqual(Offset.Z, -50.f, 0.01f)) {
		JAFGLocation.IncreaseDownwards();
	}

	return JAFGLocation;
}
