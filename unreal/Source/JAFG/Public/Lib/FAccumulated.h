// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "FAccumulated.generated.h"

class UGI_Master;

USTRUCT()
struct JAFG_API FAccumulated
{
    GENERATED_BODY()

public:

    static void Init(const UGI_Master* InGIPtr);
    
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
    /**
     * Value copied from the game instance for faster access and to not have to deal with UWorld pointers.
     * Not available during game instance initialization.
     */
    inline static int ItemIndexStart = -1;
    FORCEINLINE bool IsVoxel() const { return this->Accumulated < FAccumulated::ItemIndexStart; }
    FORCEINLINE FString ToString() const { return FString::Printf(TEXT("{Accumulated:%d, Amount:%d}"), this->Accumulated, this->Amount); }
};
