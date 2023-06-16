// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../World/FJAFGCoordinateSystem.h"

#include "AACTR_BLOCKCORE.generated.h"


UCLASS()
class JAFG_API AACTR_BLOCKCORE : public AActor {
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AACTR_BLOCKCORE();
	AACTR_BLOCKCORE(FString BlockID);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* StaticMeshComponent;

public:

	UPROPERTY(EditAnywhere)
	FString BlockID;

public:

	FVector GetBlockLocation();
	// Override this function if UStaticMesh is not a whole block.
	FJAFGCoordinateSystem GetNewBlockPositionOnHit(FHitResult HitResult);

};
