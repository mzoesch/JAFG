// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "JAFGWorldSubsystems.h"
#include "Lib/LiteNoise.h"
#include "Lib/NoiseSpline.h"

#include "ChunkWorldSettings.generated.h"

JAFG_VOID

class AEditorChunkWorldSettings;

/**
 * Contains general settings for the chunk world that only apply to the local player.
 * Will never exist on a dedicated server.
 */
UCLASS(NotBlueprintable)
class JAFG_API ULocalChunkWorldSettings : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    ULocalChunkWorldSettings();

    // WorldSubsystem implementation
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    // ~WorldSubsystem implementation

    EChunkType::Type LocalChunkType;
    FORCEINLINE auto GetLocalChunkType(void) const -> EChunkType::Type { return this->LocalChunkType; }

    // TODO Ofc make the replication for this.
    int ReplicatedChunksAboveZero = 4;
};

/**
 * Contains settings only valid on a server or standalone client.
 */
UCLASS(NotBlueprintable)
class JAFG_API UServerChunkWorldSettings : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    UServerChunkWorldSettings();
    virtual ~UServerChunkWorldSettings(void) override;

    // WorldSubsystem implementation
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    // ~WorldSubsystem implementation

    //////////////////////////////////////////////////////////////////////////
    // Settings - Generation
    //////////////////////////////////////////////////////////////////////////

    // General
    //////////////////////////////////////////////////////////////////////////
    EWorldGenerationType::Type WorldGenerationType  = EWorldGenerationType::Superflat;
    int                        ChunksAboveZero      = 4;
    float                      FakeHeightMultiplier = 0.6f;
    uint64                     Seed                 = 0;

    // Noise
    //////////////////////////////////////////////////////////////////////////

        // World
        //////////////////////////////////////////////////////////////////////////
        double             WorldFrequency   = 0.006;
        ENoiseType::Type   WorldNoiseType   = ENoiseType::Perlin;
        EFractalType::Type WorldFractalType = EFractalType::FBm;

        // Continentalness
        //////////////////////////////////////////////////////////////////////////
        double                    ContinentalnessFrequency   = 0.001;
        ENoiseType::Type          ContinentalnessNoiseType   = ENoiseType::Perlin;
        EFractalType::Type        ContinentalnessFractalType = EFractalType::FBm;
        TArray<FNoiseSplinePoint> ContinentalnessSpline      =
            {
                FNoiseSplinePoint(-1.0f,   0.0f),
                FNoiseSplinePoint( 1.0f, 100.0f),
            };

    //////////////////////////////////////////////////////////////////////////
    // Auto generated
    //////////////////////////////////////////////////////////////////////////

    FFastNoiseLite NoiseWorld           = NULL;
    FFastNoiseLite NoiseContinentalness = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Fast Common Getters
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto GetHighestPoint(void) const -> int32 { return this->ChunksAboveZero * WorldStatics::ChunkSize; }
    FORCEINLINE auto GetFakeHighestPoint(void) const -> int32 { return this->GetHighestPoint() * this->FakeHeightMultiplier; }

    FORCEINLINE auto GetWorldGenerationType(void) const -> EWorldGenerationType::Type { return this->WorldGenerationType; }


private:

#if WITH_EDITOR
    /**
     * Should always be set manually in the code directly.
     * Useful for testing setting loading, saving, etc.
     */
    bool bNeverReflectChangesFromEditorOnlySettings = false;
    /** Always call last, after all settings have already been set. */
    void ReflectChangesFromEditorOnlySettings(const AEditorChunkWorldSettings& EditorChunkWorldSettings);
#endif /* WITH_EDITOR */

};

/**
 * Editor only.
 * Change settings here in the editor to quickly test different settings without the need to recompile the application.
 *
 * Changes made here will never be reflected in the actual game.
 */
UCLASS(NotBlueprintable)
class JAFG_API AEditorChunkWorldSettings : public AWorldSettings
{
    GENERATED_BODY()

public:

    explicit AEditorChunkWorldSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    //////////////////////////////////////////////////////////////////////////
    // Settings - Generation
    //////////////////////////////////////////////////////////////////////////

    // General
    //////////////////////////////////////////////////////////////////////////
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    bool bOverrideWorldGenerationType = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideWorldGenerationType"))
    TEnumAsByte<EWorldGenerationType::Type> WorldGenerationType = EWorldGenerationType::Invalid;
    FORCEINLINE auto HasWorldGenerationTypeOverride(void) const -> bool { return this->bOverrideWorldGenerationType; }
    FORCEINLINE auto GetWorldGenerationType(void) const -> EWorldGenerationType::Type { return this->WorldGenerationType; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    bool bOverrideChunksAboveZero = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideChunksAboveZero"))
    int ChunksAboveZero = 4;
    FORCEINLINE auto HasChunksAboveZeroOverride(void) const -> bool { return this->bOverrideChunksAboveZero; }
    FORCEINLINE auto GetChunksAboveZero(void) const -> int { return this->ChunksAboveZero; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    bool bOverrideFakeHeightMultiplier = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideFakeHeightMultiplier"))
    float FakeHeightMultiplier = 0.6f;
    FORCEINLINE auto HasFakeHeightMultiplierOverride(void) const -> bool { return this->bOverrideFakeHeightMultiplier; }
    FORCEINLINE auto GetFakeHeightMultiplier(void) const -> float { return this->FakeHeightMultiplier; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    bool bOverrideSeed = false;
    UPROPERTY(EditAnywhere,  /* BlueprintReadWrite, */ Category = "Generation", meta = (EditCondition = "bOverrideSeed"))
    uint64 Seed = 0;
    FORCEINLINE auto HasSeedOverride(void) const -> bool { return this->bOverrideSeed; }
    FORCEINLINE auto GetSeed(void) const -> uint64 { return this->Seed; }

    // Noise
    //////////////////////////////////////////////////////////////////////////

        // World
        //////////////////////////////////////////////////////////////////////////
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
        bool bOverrideWorldFrequency = false;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideWorldFrequency"))
        double WorldFrequency = 0.006;
        FORCEINLINE auto HasWorldFrequencyOverride(void) const -> bool { return this->bOverrideWorldFrequency; }
        FORCEINLINE auto GetWorldFrequency(void) const -> double { return this->WorldFrequency; }

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
        bool bOverrideWorldNoiseType = false;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideWorldNoiseType"))
        TEnumAsByte<ENoiseType::Type> WorldNoiseType = ENoiseType::Perlin;
        FORCEINLINE auto HasWorldNoiseTypeOverride(void) const -> bool { return this->bOverrideWorldNoiseType; }
        FORCEINLINE auto GetWorldNoiseType(void) const -> ENoiseType::Type { return this->WorldNoiseType; }

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
        bool bOverrideWorldFractalType = false;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideWorldFractalType"))
        TEnumAsByte<EFractalType::Type> WorldFractalType = EFractalType::FBm;
        FORCEINLINE auto HasWorldFractalTypeOverride(void) const -> bool { return this->bOverrideWorldFractalType; }
        FORCEINLINE auto GetWorldFractalType(void) const -> EFractalType::Type { return this->WorldFractalType; }

        // Continentalness
        //////////////////////////////////////////////////////////////////////////
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
        bool bOverrideContinentalnessFrequency = false;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideContinentalnessFrequency"))
        double ContinentalnessFrequency = 0.001;
        FORCEINLINE auto HasContinentalnessFrequencyOverride(void) const -> bool { return this->bOverrideContinentalnessFrequency; }
        FORCEINLINE auto GetContinentalnessFrequency(void) const -> double { return this->ContinentalnessFrequency; }

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
        bool bOverrideContinentalnessNoiseType = false;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideContinentalnessNoiseType"))
        TEnumAsByte<ENoiseType::Type> ContinentalnessNoiseType = ENoiseType::Perlin;
        FORCEINLINE auto HasContinentalnessNoiseTypeOverride(void) const -> bool { return this->bOverrideContinentalnessNoiseType; }
        FORCEINLINE auto GetContinentalnessNoiseType(void) const -> ENoiseType::Type { return this->ContinentalnessNoiseType; }

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
        bool bOverrideContinentalnessFractalType = false;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideContinentalnessFractalType"))
        TEnumAsByte<EFractalType::Type> ContinentalnessFractalType = EFractalType::FBm;
        FORCEINLINE auto HasContinentalnessFractalTypeOverride(void) const -> bool { return this->bOverrideContinentalnessFractalType; }
        FORCEINLINE auto GetContinentalnessFractalType(void) const -> EFractalType::Type { return this->ContinentalnessFractalType; }

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
        bool bOverrideContinentalnessSpline = false;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideContinentalnessSpline"))
        TArray<FNoiseSplinePoint> ContinentalnessSpline =
            {
                FNoiseSplinePoint(-1.0f,   0.0f),
                FNoiseSplinePoint( 1.0f, 100.0f),
            };
        FORCEINLINE auto HasContinentalnessSplineOverride(void) const -> bool { return this->bOverrideContinentalnessSpline; }
        FORCEINLINE auto GetContinentalnessSpline(void) const -> TArray<FNoiseSplinePoint> { return this->ContinentalnessSpline; }
};
