// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Chunk.h"
#include "Lib/FastNoiseLite.h"

#include "ChunkWorld.generated.h"

// Density is a value between -1 and 1. -1 is air and 1 is stone.
// Squashing factor: How much we can show of the 3d nosie.

UENUM()
enum EWorldGenerationType : uint8
{
    WGT_SuperFlat,
    WGT_Default,
};

USTRUCT()
struct JAFG_API FNoiseSplinePoint
{
    GENERATED_BODY()

    FNoiseSplinePoint() = default;
    FNoiseSplinePoint(const float PercentHeight, const float TargetDensity) : PercentHeight(PercentHeight), TargetDensity(TargetDensity) { return; }
    
    UPROPERTY(EditInstanceOnly, meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float PercentHeight = 0.0f; 
    
    /** The percentage of the current world height. */
    UPROPERTY(EditInstanceOnly, meta = (ClampMin = "-1.0", ClampMax = "1.0"))
    float TargetDensity = 0.0f;

    static float GetDensity(const TArray<FNoiseSplinePoint>& Points, const float PercentHeight, const float X, const float Y);
};

USTRUCT()
struct JAFG_API FNoiseSplinePointV2
{
    GENERATED_BODY()

    FNoiseSplinePointV2() = default;
    FNoiseSplinePointV2(const float NoiseValue, const float TargetPercentageTerrainHeight) : NoiseValue(NoiseValue), TargetPercentageTerrainHeight(TargetPercentageTerrainHeight) { return; }
    
    UPROPERTY(EditInstanceOnly, meta = (ClampMin = "-1.0", ClampMax = "1.0"))
    float NoiseValue = 0.0f; 
    
    /** The percentage of the current world height. */
    UPROPERTY(EditInstanceOnly, meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float TargetPercentageTerrainHeight = 0.0f;

    static float GetDensity(const TArray<FNoiseSplinePoint>& Points, const float PercentHeight, const float X, const float Y);
};

UCLASS()
class JAFG_API AChunkWorld : public AActor
{
    GENERATED_BODY()
    
public:	

    AChunkWorld();

public:
    
    UPROPERTY(EditInstanceOnly, Category="Generation")
    TEnumAsByte<EWorldGenerationType> WorldGenerationType = EWorldGenerationType::WGT_SuperFlat;

    UPROPERTY(EditInstanceOnly, Category="Generation")
    uint64 Seed = 0;
    
    UPROPERTY(EditInstanceOnly, Category="Generation")
    int DetailedDrawDistance = 1;

    UPROPERTY(EditInstanceOnly, Category="Generation")
    int ChunksAboveZero = 3;
    
    UPROPERTY(EditInstanceOnly, Category="Generation")
    int ChunksBelowZero = 3;

    UPROPERTY(EditInstanceOnly, Category="Generation")
    float FakeHeightMultiplier = 1.6f;
    
    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    double WorldFrequency = 0.03;

    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    bool bInvertWorld = false;
    
    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    TEnumAsByte<EFractalType> WorldFractalType = EFractalType::FractalType_FBm;
    
    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    TEnumAsByte<ENoiseType> WorldNoiseType = ENoiseType::NoiseType_Perlin;

    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    double ContinentalnessFrequency = 0.03;

    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    bool bInvertContinentalness = false;
    
    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    TEnumAsByte<EFractalType> ContinentalnessFractalType = EFractalType::FractalType_FBm;
    
    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    TEnumAsByte<ENoiseType> ContinentalnessNoiseType = ENoiseType::NoiseType_Perlin;
    
    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    TArray<FNoiseSplinePointV2> ContinentalnessSplinePoints =
    {
        FNoiseSplinePointV2(    -1.0f  ,  100.0f    ),
        FNoiseSplinePointV2(    -0.9f  ,    0.0f    ),
        FNoiseSplinePointV2(    -0.3f  ,    0.0f    ),
        FNoiseSplinePointV2(     0.0f  ,   30.0f    ),
        FNoiseSplinePointV2(     0.3f  ,   30.0f    ),
        FNoiseSplinePointV2(     0.5f  ,   90.0f    ),
        FNoiseSplinePointV2(     0.8f  ,   95.0f    ),
        FNoiseSplinePointV2(     1.0f  ,  100.0f    ),
    };

    UPROPERTY(EditInstanceOnly, Category="Generation|Noise|FeaturesAndStructures")
    bool bAddTrees = true;

    UPROPERTY(EditInstanceOnly, Category="Generation|Noise|FeaturesAndStructures")
    float TreeFrequency = 0.5f;

    /**
     * If the density at a given 2D point in the word is below this threshold
     * and the world generator is allowed to place a tree at this location,
     * a tree will be spawned.
     */
    UPROPERTY(EditInstanceOnly, Category="Generation|Noise|FeaturesAndStructures")
    float TreeThreshold = -0.2f;
    
    UPROPERTY(EditInstanceOnly, Category="Generation|Noise|FeaturesAndStructures")
    TEnumAsByte<EFractalType> TreeFractalType = EFractalType::FractalType_None;

    UPROPERTY(EditInstanceOnly, Category="Generation|Noise|FeaturesAndStructures")
    TEnumAsByte<ENoiseType> TreeNoiseType = ENoiseType::NoiseType_Value;
    
public:

    FastNoiseLite* NWorld;
    FastNoiseLite* NContinentalness;
    FastNoiseLite* NTree;

protected:

    virtual void BeginPlay() override;

public:	

    virtual void Tick(const float DeltaTime) override;

private:

    void GenerateWorldAsync();
    void GenerateWorld();

public:

    FORCEINLINE int GetHighestPointV2() const   { return  this->ChunksAboveZero * AChunk::CHUNK_SIZE; }
    FORCEINLINE int GetLowestPointV2()  const   { return -this->ChunksBelowZero * AChunk::CHUNK_SIZE; }
    FORCEINLINE int GetWorldHeightV2() const    { return this->GetHighestPointV2() - this->GetLowestPointV2(); }
    
    AChunk* GetChunkByKey(const FIntVector& Key) const;
    
public:

    static FIntVector WorldToChunkPosition(const FVector& WorldPosition);
    static FIntVector WorldToLocalVoxelPosition(const FVector& WorldPosition);

private:

    UPROPERTY()
    TMap<FIntVector, AChunk*> LoadedChunks;

    TQueue<FIntVector> ChunkGenerationQueue;

};
