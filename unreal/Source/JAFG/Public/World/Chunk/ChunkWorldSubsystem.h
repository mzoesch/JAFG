// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Subsystems/WorldSubsystem.h"
#include "Lib/FastNoiseLite.h"

#include "ChunkWorldSubsystem.generated.h"

JAFG_VOID

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

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Generation")
    EChunkType ChunkType = EChunkType::Greedy;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Generation")
    EWorldGenerationType WorldGenerationType = EWorldGenerationType::SuperFlat;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Generation")
    int ChunksAboveZero = 3;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Generation")
    int MaxSpiralPoints = 20;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Generation|Noise")
    double WorldFrequency = 0.006;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Generation|Noise")
    EKismetNoiseType NoiseType = EKismetNoiseType::Perlin;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Generation|Noise|Continentalness")
    double ContinentalnessFrequency = 0.001;
};

UCLASS(NotBlueprintable)
class JAFG_API UChunkWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:

    // Word Subsystem implementation
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void PostInitialize(void) override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void Deinitialize(void) override;
    // ~Word Subsystem implementation
};
