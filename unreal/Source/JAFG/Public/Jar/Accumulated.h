// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "World/Voxel/CommonVoxels.h"

#include "Accumulated.generated.h"

JAFG_VOID

#define DEFAULT_SLOT_SAFE_ADD_POST_BEHAVIOR(State, Slot, SlotArray) \
    if (State == false)                                             \
    {                                                               \
        LOG_FATAL(                                                  \
            LogTemp, "Safe Add failed for %d slot. Found %s.",      \
            Slot, *SlotArray[Slot].Content.ToString()               \
        )                                                           \
        return;                                                     \
    }

namespace Accumulated
{

/**
 * Value copied from the voxel subsystem for faster access and to not have to deal with UWorld pointers.
 * Not available before the voxel subsystem is completely initialized.
 */
inline static int32 ItemIndexStart = -1;

FORCEINLINE static void Initialize(const int32 InItemIndexStart)
{
    Accumulated::ItemIndexStart = InItemIndexStart;

    check( Accumulated::ItemIndexStart >= 0 )

    return;
}

}

USTRUCT()
struct JAFG_API FAccumulated
{
    GENERATED_BODY()

    FAccumulated(void);
    explicit FAccumulated(const int32 InAccumulatedIndex);
    explicit FAccumulated(const int32 InAccumulatedIndex, const uint16 InAmount);

    UPROPERTY( /* Replicated */ )
    int32  AccumulatedIndex;
    UPROPERTY( /* Replicated */ )
    uint16 Amount;

    /** Checks for overflow, underflow and if post add amount is zero, the Null Accumulated is set. */
    void SafeAddAmount(const int32 InAmount, bool& bCouldProcess);

    /** Does not compare amount. */
    FORCEINLINE        auto operator==(const FAccumulated& O) const -> bool { return this->AccumulatedIndex == O.AccumulatedIndex; }
    /** Does not compare amount. */
    FORCEINLINE static auto Equals(const FAccumulated& A, const FAccumulated& B) -> bool { return A == B; }
    FORCEINLINE        auto IsVoxel(void) const -> bool { return this->AccumulatedIndex < Accumulated::ItemIndexStart; }
    FORCEINLINE        auto ToString(void) const -> FString
    {
        return FString::Printf(TEXT("FAccumulated{AccumulatedIndex:%d, Amount:%d}"), this->AccumulatedIndex, this->Amount);
    }
};

namespace Accumulated
{

static const FAccumulated Null { ECommonVoxels::Null, 0 };

}
