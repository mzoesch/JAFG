// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "WorldCore/JAFGWorldSubsystems.h"
#include "Lib/LiteNoise.h"
#include "Lib/NoiseSpline.h"

#include "ChunkWorldSettings.generated.h"

JAFG_VOID

class UServerWorldSettingsReplicationComponent;
class UServerChunkWorldSettings;
class AWorldCharacter;
class AEditorChunkWorldSettings;

USTRUCT()
struct FReplicatedServerWorldSettings
{
    GENERATED_BODY()

    UPROPERTY( /* Replicated */ )
    int32 ChunksAboveZero = -1;
};

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

    FORCEINLINE auto GetReplicatedChunksAboveZero(void) const -> int32 { return this->ReplicatedChunksAboveZero; }

private:

    friend UServerWorldSettingsReplicationComponent;

    int32 ReplicatedChunksAboveZero = -1;
};

UCLASS(NotBlueprintable)
class UServerWorldSettingsReplicationComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    explicit UServerWorldSettingsReplicationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UActorComponent implementation
    virtual void BeginPlay(void) override;
    // ~UActorComponent implementation

    UFUNCTION(Server, Reliable)
    void RequestRepData_ServerRPC( /* void */ );
    UFUNCTION(Client, Reliable)
    void RepSettings_ClientRPC(const FReplicatedServerWorldSettings& ReplicatedServerWorldSettings);
    /** Call from authority to share specific world configs that clients must know. */
    void Rep_Settings(const UServerChunkWorldSettings* ServerChunkWorldSettings);

    bool HasReplicatedSettings(void) const { return this->bHasReplicatedSettings; }

private:

    /** Only meaningful on a client. */
    bool bHasReplicatedSettings = false;
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
    EWorldGenerationType::Type WorldGenerationType  = EWorldGenerationType::Default;
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

        // Cheese Cave
        //////////////////////////////////////////////////////////////////////////
        double             CheeseCaveFrequency   = 0.006;
        ENoiseType::Type   CheeseCaveNoiseType   = ENoiseType::Perlin;
        EFractalType::Type CheeseCaveFractalType = EFractalType::FBm;

    //////////////////////////////////////////////////////////////////////////
    // Auto generated
    //////////////////////////////////////////////////////////////////////////

    FFastNoiseLite NoiseWorld           = NULL;
    FFastNoiseLite NoiseContinentalness = NULL;
    FFastNoiseLite NoiseCheeseCave      = NULL;

    //////////////////////////////////////////////////////////////////////////
    // Fast Common Getters
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto GetHighestPoint(void) const -> int32 { return this->ChunksAboveZero * WorldStatics::ChunkSize; }
    FORCEINLINE auto GetFakeHighestPoint(void) const -> int32 { return this->GetHighestPoint() * this->FakeHeightMultiplier; }

    FORCEINLINE auto GetWorldGenerationType(void) const -> EWorldGenerationType::Type { return this->WorldGenerationType; }

private:

#if WITH_EDITOR
    /** Always call last, after all settings have already been set. */
    void ReflectChangesFromEditorOnlySettings(const AEditorChunkWorldSettings& EditorChunkWorldSettings);
#endif /* WITH_EDITOR */

};

/**
 * Editor only.
 * Change settings here in the editor to quickly test different settings without the need to recompile the application.
 *
 * Changes made here will never be reflected in a standalone game or server.
 */
UCLASS(NotBlueprintable)
class JAFG_API AEditorChunkWorldSettings : public AWorldSettings
{
    GENERATED_BODY()

public:

    explicit AEditorChunkWorldSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // AActor implementation
    virtual void BeginPlay(void) override;
    // ~AActor implementation

    //////////////////////////////////////////////////////////////////////////
    // Settings - Player
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    bool bShowDebugScreenWhenSimulating = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    int32 SimulationRenderDistance = 12;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
    TSubclassOf<APawn> PendingPawn = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
    TSubclassOf<AWorldCharacter> CharacterToUse = nullptr;

    //////////////////////////////////////////////////////////////////////////
    // Settings - Generation
    //////////////////////////////////////////////////////////////////////////

    // General
    //////////////////////////////////////////////////////////////////////////
    /** Editor only. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    bool bReflectSettingsToGenerator = true;

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

        // Cheese Cave
        //////////////////////////////////////////////////////////////////////////
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
        bool bOverrideCheeseCaveFrequency = false;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideCheeseCaveFrequency"))
        double CheeseCaveFrequency = 0.006;
        FORCEINLINE auto HasCheeseCaveFrequencyOverride(void) const -> bool { return this->bOverrideCheeseCaveFrequency; }
        FORCEINLINE auto GetCheeseCaveFrequency(void) const -> double { return this->CheeseCaveFrequency; }

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
        bool bOverrideCheeseCaveNoiseType = false;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideCheeseCaveNoiseType"))
        TEnumAsByte<ENoiseType::Type> CheeseCaveNoiseType = ENoiseType::Perlin;
        FORCEINLINE auto HasCheeseCaveNoiseTypeOverride(void) const -> bool { return this->bOverrideCheeseCaveNoiseType; }
        FORCEINLINE auto GetCheeseCaveNoiseType(void) const -> ENoiseType::Type { return this->CheeseCaveNoiseType; }

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
        bool bOverrideCheeseCaveFractalType = false;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "bOverrideCheeseCaveFractalType"))
        TEnumAsByte<EFractalType::Type> CheeseCaveFractalType = EFractalType::FBm;
        FORCEINLINE auto HasCheeseCaveFractalTypeOverride(void) const -> bool { return this->bOverrideCheeseCaveFractalType; }
        FORCEINLINE auto GetCheeseCaveFractalType(void) const -> EFractalType::Type { return this->CheeseCaveFractalType; }
};
