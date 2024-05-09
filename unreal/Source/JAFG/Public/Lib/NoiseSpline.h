// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "NoiseSpline.generated.h"

/**
 * Maps a noise value to a target percentage of the terrain height.
 * Useful to map an array of many noise values to a terrain height over all possible noise values.
 */
USTRUCT(BlueprintType)
struct JAFG_API FNoiseSplinePoint
{
    GENERATED_BODY()

    FNoiseSplinePoint(void) = default;
    explicit FNoiseSplinePoint(const float NoiseValue, const float TargetPercentageTerrainHeight)
    : NoiseValue(NoiseValue), TargetPercentageTerrainHeight(TargetPercentageTerrainHeight)
    {
        return;
    }

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (ClampMin = "-1.0", ClampMax = "1.0"))
    float NoiseValue = 0.0f;

    /** The percentage of the current world height. */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "100.0"))
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

    return (
          ((1 - Distance) * Lower->TargetPercentageTerrainHeight)
        + (Distance * Upper->TargetPercentageTerrainHeight)
    ) / 100.0f;
}

}
