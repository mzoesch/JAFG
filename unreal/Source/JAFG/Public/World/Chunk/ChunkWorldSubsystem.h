// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Subsystems/WorldSubsystem.h"
#include "Lib/FastNoiseLite.h"

#include "ChunkWorldSubsystem.generated.h"

JAFG_VOID

class AChunkMulticasterInfo;

/*
 * General information:
 *
 *  Density:
 *      - Value between -1 and 1.
 *      - Air is -1 and base voxel is 1.
 */

UENUM(BlueprintType)
enum class EChunkType
{
    Naive,
    Greedy,
    Marching,
};

UENUM(BlueprintType)
enum class EWorldGenerationType
{
    Default,
    SuperFlat,
};

USTRUCT()
struct JAFG_API FNoiseSplinePoint
{
    GENERATED_BODY()

    FNoiseSplinePoint(void) = default;
    explicit FNoiseSplinePoint(const float NoiseValue, const float TargetPercentageTerrainHeight)
    : NoiseValue(NoiseValue), TargetPercentageTerrainHeight(TargetPercentageTerrainHeight)
    {
        return;
    }

    UPROPERTY(EditInstanceOnly, meta = (ClampMin = "-1.0", ClampMax = "1.0"))
    float NoiseValue = 0.0f;

    /** The percentage of the current world height. */
    UPROPERTY(EditInstanceOnly, meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float TargetPercentageTerrainHeight = 0.0f;
};

namespace NoiseSpline
{

/**
 * @return The target height that this spline aims for at the given noise value ranging from 0.0 to 1.0.
 *         0.0 for the lowest point and 1.0 for the highest point.
 */
FORCEINLINE static float GetTargetHeight(const TArray<FNoiseSplinePoint>& Spline, const float Noise)
{
    const FNoiseSplinePoint* Lower = nullptr;
    const FNoiseSplinePoint* Upper = nullptr;

    for (int i = 1; i < Spline.Num(); ++i)
    {
        if (Spline[i].NoiseValue >= Noise)
        {
            Lower = &Spline[i - 1];
            Upper = &Spline[i];
            break;
        }
    }

    const float Distance = (Noise - Lower->NoiseValue) / (Upper->NoiseValue - Lower->NoiseValue);

    return /*1 - */(((1 - Distance) * Lower->TargetPercentageTerrainHeight) + (Distance * Upper->TargetPercentageTerrainHeight)) / 100.0f;
}

}

namespace ChunkWorldSettings
{

/**
 * Do not increase this value beyond 16 for now. As we would breach the Bunch size limit of 2^16 = 65.536 bytes.
 * See CommonChunk.h for more information.
 */
inline static constexpr int    ChunkSize        { 16                                         };

inline static constexpr float  JToUScale        { 100.0f                                     };
inline static constexpr float  UToJScale        { 1.0f / ChunkWorldSettings::JToUScale       };
inline static constexpr double JToUScaleDouble  { 100.0                                      };
inline static constexpr double UToJScaleDouble  { 1.0  / ChunkWorldSettings::JToUScaleDouble };
inline static constexpr int    JToUScaleInteger { 100                                        };
/* There is obviously no integer with a U To J Scale. */

FORCEINLINE FString LexToString(const EWorldGenerationType Type)
{
    switch (Type)
    {
    case EWorldGenerationType::Default:
    {
        return FString(TEXT("Default"));
    }
    case EWorldGenerationType::SuperFlat:
    {
        return FString(TEXT("SuperFlat"));
    }
    default:
    {
        return FString(TEXT("Unknown"));
    }
    }
}

}

UCLASS()
class AChunkWorldSettings : public AWorldSettings
{
    GENERATED_BODY()

public:

    //////////////////////////////////////////////////////////////////////////
    // Note that all these member variables are only for testing in the
    // editor. Changing them will not have any effect on the actual shipped
    // game. We load the settings from a file on the server based on the
    // current world type.
    // More in detail: The settings are loaded bases on the current save file
    // and the world type. This implies that these settings will never be
    // valid on any other instance than the authoritative server.
    // See UChunkWorldSubsystem::Initialize for more information.
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation")
    EChunkType ChunkType = EChunkType::Greedy;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation")
    EWorldGenerationType WorldGenerationType = EWorldGenerationType::SuperFlat;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation")
    int ChunksAboveZero = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation")
    float FakeHeightMultiplier = 0.6f;

    UPROPERTY(EditAnywhere, /* BlueprintReadOnly, */ Category = "Generation")
    uint64 Seed = 0;

    /**
     * How many chunks the server may generate at one tick. Clients will have to wait for the server to generate
     * chunks if the waiting queue is bigger than this value.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation|Runtime")
    int MaxServerChunksPerTick = 100;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation|Runtime")
    bool bOverrideServerChunkGenerationTickRate = false;

    /**
     * If less than or equal to 0.0f the server will generate chunks as fast as possible. Every tick will be maxed out
     * based on the AChunkWorldSettings#MaxServerChunksPerTick value.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation|Runtime")
    float ServerChunkGenerationTickRate = 0.0f;

    /**
     * May be more. As we only check at the end of one round around the circle in the middle
     * of the spiral.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation|Mocking")
    int MaxSpiralPoints = 20;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation|Noise|World")
    double WorldFrequency = 0.006;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation|Noise|World")
    EKismetNoiseType WorldKismetNoiseType = EKismetNoiseType::Perlin;
    ENoiseType::Type WorldNoiseType       = ENoiseType::Perlin;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation|Noise|World")
    EKismetFractalType WorldKismetFractalType = EKismetFractalType::FBm;
    EFractalType::Type WorldFractalType       = EFractalType::FBm;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation|Noise|Continentalness")
    double ContinentalnessFrequency = 0.001;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation|Noise|Continentalness")
    EKismetNoiseType ContinentalnessKismetNoiseType = EKismetNoiseType::Perlin;
    ENoiseType::Type ContinentalnessNoiseType       = ENoiseType::Perlin;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generation|Noise|Continentalness")
    EKismetFractalType ContinentalnessKismetFractalType = EKismetFractalType::FBm;
    EFractalType::Type ContinentalnessFractalType       = EFractalType::FBm;

    UPROPERTY(EditAnywhere, /* BlueprintReadOnly, */ Category = "Generation|Noise|Continentalness")
    TArray<FNoiseSplinePoint> ContinentalnessSpline = {};

    //////////////////////////////////////////////////////////////////////////
    // Auto generated
    //////////////////////////////////////////////////////////////////////////

    FFastNoiseLite* NoiseWorld = nullptr;
    FFastNoiseLite* NoiseContinentalness = nullptr;


    FORCEINLINE int32 GetHighestPoint(void) const
    {
        return ChunksAboveZero * ChunkWorldSettings::ChunkSize;
    }

    FORCEINLINE int32 GetFakeHighestPoint(void) const
    {
        return this->GetHighestPoint() * FakeHeightMultiplier;
    }
};

UCLASS(NotBlueprintable)
class JAFG_API UChunkWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:

    // World Subsystem implementation
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void PostInitialize(void) override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void Deinitialize(void) override;
    // ~World Subsystem implementation

    void BroadcastChunkModification(const FIntVector& ChunkKey, const FIntVector& LocalVoxelLocation, const int32 Voxel) const;

private:

    UPROPERTY()
    TObjectPtr<AChunkMulticasterInfo> ChunkMulticasterInfo = nullptr;
    UPROPERTY()
    TObjectPtr<AChunkWorldSettings> ChunkWorldSettings = nullptr;
};
