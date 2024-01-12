// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Chunk.h"
#include "Lib/FastNoiseLite.h"

#include "ChunkWorld.generated.h"

UENUM()
enum EWorldGenerationType : uint8
{
    WGT_SuperFlat,
    WGT_Legacy,
    WGT_Pure3DDefault,
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
    int DrawHeight = 7;

    UPROPERTY(EditInstanceOnly, Category="Generation")
    int ChunksBelowZero = 3;

    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    double ContinentalnessFrequency = 0.03;

    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    bool bInvertContinentalness = false;
    
    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    TEnumAsByte<EFractalType> ContinentalnessFractalType = EFractalType::FractalType_FBm;
    
    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    TEnumAsByte<ENoiseType> ContinentalnessNoiseType = ENoiseType::NoiseType_Perlin;

    // -1 : stone
    //  1 : air
    UPROPERTY(EditInstanceOnly, Category="Generation|Noise")
    TArray<FNoiseSplinePoint> NoiseSplinePoints =
    {
        FNoiseSplinePoint(    0.0f  ,  -1.0f   ),
        FNoiseSplinePoint(   40.0f  ,  -1.0f   ),
        FNoiseSplinePoint(   45.0f  ,  -0.2f   ),
        FNoiseSplinePoint(   50.0f  ,   0.0f   ),
        FNoiseSplinePoint(   70.0f  ,   0.2f   ),
        FNoiseSplinePoint(   80.0f  ,   0.5f   ),
        FNoiseSplinePoint(   95.0f  ,   0.6f   ),
        FNoiseSplinePoint(  100.0f  ,   0.75f  ),
    };
    
public:

    FastNoiseLite* NContinentalness;
    
public:

    int GetLowestPoint()    const   { return -(this->ChunksBelowZero * AChunk::CHUNK_SIZE);                                 }
    int GetHighestPoint()   const   { return ((this->DrawHeight - this->ChunksBelowZero + 1) * AChunk::CHUNK_SIZE) - 1;     }
    int GetWorldHeight()    const   { return abs(this->GetHighestPoint() - this->GetLowestPoint() + 1);                     }
    
protected:

    virtual void BeginPlay() override;

public:	

    virtual void Tick(const float DeltaTime) override;

private:

    void GenerateWorld();

public:

    static FIntVector WorldToLocalVoxelPosition(const FVector& WorldPosition);

private:

    UPROPERTY()
    TMap<FIntVector, AChunk*> LoadedChunks;

public:

    float GetDensity(const float X, const float Y, const float Z) const;
    float GenDensity(const float ContinentalnessNoise, const float WorldZ);    
};
