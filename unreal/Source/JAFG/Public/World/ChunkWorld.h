// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ChunkWorld.generated.h"

UCLASS()
class JAFG_API AChunkWorld : public AActor
{
	GENERATED_BODY()
	
public:	

	AChunkWorld();

public:

	UPROPERTY(EditInstanceOnly, Category="Material")
	const TObjectPtr<UMaterialInterface> DevMaterial;

	UPROPERTY(EditInstanceOnly, Category="Material")
	const TObjectPtr<UMaterialInterface> TranslucentMaterial;

	UPROPERTY(EditInstanceOnly, Category="Generation")
	int DetailedDrawDistance = 1;

	UPROPERTY(EditInstanceOnly, Category="Generation")
	int DrawHeight = 7;

	UPROPERTY(EditInstanceOnly, Category="Generation")
	int ChunksBelowZero = 3;

public:
	
	static inline constexpr float DevFrequency{0.03f};
	
protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(const float DeltaTime) override;

private:

	void GenerateWorld();

public:

	static FIntVector WorldToLocalVoxelPosition(const FVector& WorldPosition);
	
};
