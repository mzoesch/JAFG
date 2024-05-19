// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/ChunkWorldSettings.h"

ULocalChunkWorldSettings::ULocalChunkWorldSettings(void) : Super()
{
    this->LocalChunkType = EChunkType::Greedy;

    return;
}

bool ULocalChunkWorldSettings::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    if (UNetStatics::IsSafeDedicatedServer(Outer))
    {
        return false;
    }

    return true;
}

UServerChunkWorldSettings::UServerChunkWorldSettings(void) : Super()
{
    return;
}

UServerChunkWorldSettings::~UServerChunkWorldSettings(void)
{
    return;
}

bool UServerChunkWorldSettings::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    if (UNetStatics::IsSafeClient(Outer))
    {
        return false;
    }

    return true;
}

void UServerChunkWorldSettings::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

#if WITH_EDITOR
    if (this->bNeverReflectChangesFromEditorOnlySettings == false)
    {
        if (const AEditorChunkWorldSettings* EditorChunkWorldSettings = Cast<AEditorChunkWorldSettings>(InWorld.GetWorldSettings()); EditorChunkWorldSettings != nullptr)
        {
            this->ReflectChangesFromEditorOnlySettings(*EditorChunkWorldSettings);
        }
        else
        {
            LOG_ERROR(LogChunkMisc, "Could not cast world settings to Editor Chunk World Settings.")
        }
    }
#endif /* WITH_EDITOR */

    this->NoiseWorld = FFastNoiseLite();
    this->NoiseWorld.SetSeed(this->Seed);
    this->NoiseWorld.SetFrequency(this->WorldFrequency);
    this->NoiseWorld.SetFractalType(this->WorldFractalType);

    this->NoiseContinentalness = FFastNoiseLite();
    this->NoiseContinentalness.SetSeed(this->Seed);
    this->NoiseContinentalness.SetFrequency(this->ContinentalnessFrequency);
    this->NoiseContinentalness.SetFractalType(this->ContinentalnessFractalType);

    return;
}

#if WITH_EDITOR
void UServerChunkWorldSettings::ReflectChangesFromEditorOnlySettings(const AEditorChunkWorldSettings& EditorChunkWorldSettings)
{
    if (EditorChunkWorldSettings.HasWorldGenerationTypeOverride())
    {
        LOG_WARNING(
            LogChunkMisc,
            "Found override for world generation type. Value: [%s].",
            *EWorldGenerationType::LexToString(EditorChunkWorldSettings.GetWorldGenerationType())
        )
        this->WorldGenerationType = EditorChunkWorldSettings.GetWorldGenerationType();
    }

    if (EditorChunkWorldSettings.HasChunksAboveZeroOverride())
    {
        LOG_WARNING(
            LogChunkMisc,
            "Found override for chunks above zero. Value: [%d].",
            EditorChunkWorldSettings.GetChunksAboveZero()
        )
        this->ChunksAboveZero = EditorChunkWorldSettings.GetChunksAboveZero();
    }

    if (EditorChunkWorldSettings.HasFakeHeightMultiplierOverride())
    {
        LOG_WARNING(
            LogChunkMisc,
            "Found override for fake height multiplier. Value: [%f].",
            EditorChunkWorldSettings.GetFakeHeightMultiplier()
        )
        this->FakeHeightMultiplier = EditorChunkWorldSettings.GetFakeHeightMultiplier();
    }

    if (EditorChunkWorldSettings.HasSeedOverride())
    {
        LOG_WARNING(
            LogChunkMisc,
            "Found override for seed. Value: [%d].",
            EditorChunkWorldSettings.GetSeed()
        )
        this->Seed = EditorChunkWorldSettings.GetSeed();
    }

    if (EditorChunkWorldSettings.HasWorldFrequencyOverride())
    {
        LOG_WARNING(
            LogChunkMisc,
            "Found override for world frequency. Value: [%f].",
            EditorChunkWorldSettings.GetWorldFrequency()
        )
        this->WorldFrequency = EditorChunkWorldSettings.GetWorldFrequency();
    }

    if (EditorChunkWorldSettings.HasWorldNoiseTypeOverride())
    {
        LOG_WARNING(
            LogChunkMisc,
            "Found override for world noise type. Value: [%s].",
            *ENoiseType::LexToString(EditorChunkWorldSettings.GetWorldNoiseType())
        )
        this->WorldNoiseType = EditorChunkWorldSettings.GetWorldNoiseType();
    }

    if (EditorChunkWorldSettings.HasWorldFractalTypeOverride())
    {
        LOG_WARNING(
            LogChunkMisc,
            "Found override for world fractal type. Value: [%s].",
            *EFractalType::LexToString(EditorChunkWorldSettings.GetWorldFractalType())
        )
        this->WorldFractalType = EditorChunkWorldSettings.GetWorldFractalType();
    }

    if (EditorChunkWorldSettings.HasContinentalnessFrequencyOverride())
    {
        LOG_WARNING(
            LogChunkMisc,
            "Found override for continentalness frequency. Value: [%f].",
            EditorChunkWorldSettings.GetContinentalnessFrequency()
        )
        this->ContinentalnessFrequency = EditorChunkWorldSettings.GetContinentalnessFrequency();
    }

    if (EditorChunkWorldSettings.HasContinentalnessNoiseTypeOverride())
    {
        LOG_WARNING(
            LogChunkMisc,
            "Found override for continentalness noise type. Value: [%s].",
            *ENoiseType::LexToString(EditorChunkWorldSettings.GetContinentalnessNoiseType())
        )
        this->ContinentalnessNoiseType = EditorChunkWorldSettings.GetContinentalnessNoiseType();
    }

    if (EditorChunkWorldSettings.HasContinentalnessFractalTypeOverride())
    {
        LOG_WARNING(
            LogChunkMisc,
            "Found override for continentalness fractal type. Value: [%s].",
            *EFractalType::LexToString(EditorChunkWorldSettings.GetContinentalnessFractalType())
        )
        this->ContinentalnessFractalType = EditorChunkWorldSettings.GetContinentalnessFractalType();
    }

    if (EditorChunkWorldSettings.HasContinentalnessSplineOverride())
    {
        LOG_WARNING(
            LogChunkMisc,
            "Found override for continentalness spline. Value: [???]."
        )
        this->ContinentalnessSpline = EditorChunkWorldSettings.GetContinentalnessSpline();
    }

    return;
}
#endif /* WITH_EDITOR */

AEditorChunkWorldSettings::AEditorChunkWorldSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}
