// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Subsystems/WorldSubsystem.h"

#include "EntityWorldSubsystem.generated.h"

JAFG_VOID

struct FAccumulated;

UCLASS(NotBlueprintable)
class JAFG_API UEntityWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

    inline static constexpr float MinDefaultForceMultiplier { 250000.0f };
    inline static constexpr float MaxDefaultForceMultiplier { 500000.0f };

public:

    explicit UEntityWorldSubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // World Subsystem implementation
    virtual auto ShouldCreateSubsystem(UObject* Outer) const -> bool override;
    virtual auto Initialize(FSubsystemCollectionBase& Collection) -> void override;
    virtual auto PostInitialize(void) -> void override;
    virtual auto OnWorldBeginPlay(UWorld& InWorld) -> void override;
    virtual auto Deinitialize(void) -> void override;
    // ~World Subsystem implementation

    /**
     * Server only.
     * @param Force Is always normalized. That is why we have a multiplier.
     */
    auto CreateDrop(const FAccumulated& Accumulated, const FVector& Location, const FVector& Force, const float ForceMultiplier) const -> void;

    /**
     * Server only.
     * @param Force Is always normalized. The multiplier is calculated on a random basis on the fly.
     */
    FORCEINLINE auto CreateDrop(const FAccumulated& Accumulated, const FVector& Location, const FVector& Force) const -> void
    {
        return this->CreateDrop(
            Accumulated,
            Location,
            Force,
            FMath::FRandRange(UEntityWorldSubsystem::MinDefaultForceMultiplier, UEntityWorldSubsystem::MaxDefaultForceMultiplier)
        );
    }

    /**
     * Server only.
     * The force will be calculated based on the source transform.
     */
    FORCEINLINE auto CreateDrop(const FAccumulated& Accumulated, const FTransform& SourceTransform) const -> void
    {
        return this->CreateDrop(
            Accumulated,
            SourceTransform.GetLocation() + SourceTransform.GetRotation().GetForwardVector() * 80.0f,
            SourceTransform.GetRotation().Vector()
        );
    }

    FORCEINLINE static FVector GetRandomForceVector(const float MinRangeXY, const float MaxRangeXY, const float MinRangeZ, const float MaxRangeZ)
    {
        return FVector(
            FMath::FRandRange(MinRangeXY, MaxRangeXY),
            FMath::FRandRange(MinRangeXY, MaxRangeXY),
            FMath::FRandRange(MinRangeZ, MaxRangeZ)
        ).GetSafeNormal();
    }

    FORCEINLINE static FVector GetRandomForceVector(void)
    {
        return UEntityWorldSubsystem::GetRandomForceVector(-1.0f, 1.0f, -1.0f, 1.0f);
    }

    FORCEINLINE static FVector2D GetRandomForceVector2D(void)
    {
        return FVector2D(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f)).GetSafeNormal();
    }
};
