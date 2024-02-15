// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Lib/FAccumulated.h"

#include "EntityMaster.generated.h"

/** Acts like an interface for common entity logic. */
UCLASS()
class JAFG_API AEntityMaster : public AActor
{
    GENERATED_BODY()

public:

    AEntityMaster();

public:

    static constexpr float MinDefaultForceMultiplier { 250000.0f };
    static constexpr float MaxDefaultForceMultiplier { 500000.0f };

    /** The force is always normalized. */
    bool CreateDrop(const FAccumulated& Accumulated, const FVector& Location, const FVector& Force, const float ForceMultiplier) const;
    /** The force is always normalized. */
    inline bool CreateDrop(const FAccumulated& Accumulated, const FVector& Location, const FVector& Force) const;
    /** The force will be calculated based on the source transform. */
    inline bool CreateDrop(const FAccumulated& Accumulated, const FTransform& SourceTransform) const;

    FORCEINLINE static FVector GetRandomForceVector(const float MinRangeXY, const float MaxRangeXY, const float MinRangeZ, const float MaxRangeZ)
    { return FVector(FMath::FRandRange(MinRangeXY, MaxRangeXY), FMath::FRandRange(MinRangeXY, MaxRangeXY), FMath::FRandRange(MinRangeZ, MaxRangeZ)).GetSafeNormal(); }
    FORCEINLINE static FVector GetRandomForceVector()
    { return AEntityMaster::GetRandomForceVector(-1.0f, 1.0f, -1.0f, 1.0f); }
    FORCEINLINE static FVector2d GetRandomForceVector2D()
    { return FVector2d(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f)).GetSafeNormal(); }
};
