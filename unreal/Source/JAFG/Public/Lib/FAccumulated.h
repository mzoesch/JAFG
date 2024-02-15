// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "FAccumulated.generated.h"

USTRUCT()
struct JAFG_API FAccumulated
{
    GENERATED_BODY()

public:

    FAccumulated();
    explicit FAccumulated(const int AccumulatedIndex);
    explicit FAccumulated(const int AccumulatedIndex, const int InAmount);

public:

    int         Accumulated;
    uint16_t    Amount;

public:

    static const FAccumulated NullAccumulated;
    
public:

    /** Checks for overflow, underflow and if post add amount is zero NullAccumulated is set. */
    void SafeAddAmount(const int InAmount, bool& bCouldProcess);
    
    /** Does not compare amount. */
    FORCEINLINE bool operator==(const FAccumulated& O) const { return this->Accumulated == O.Accumulated; }
    /** Does not compare amount. */
    FORCEINLINE static bool Equals(const FAccumulated& A, const FAccumulated& B) { return A == B; }
    /** TODO OFC Very sketchy. There must be a better solution than hard coding this. */
    FORCEINLINE bool IsVoxel() const { return this->Accumulated < 7; }
    FORCEINLINE FString ToString() const { return FString::Printf(TEXT("FAccumulated{Accumulated:%d,Amount:%d}"), this->Accumulated, this->Amount); }
};
