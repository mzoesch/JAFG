// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGTypes.h"

#include "Accumulated.generated.h"

/**
 * Value copied from the voxel subsystem for faster access and to not have to deal with UWorld pointers.
 * Not available before the voxel subsystem is completely initialized.
 */
extern JAFGEXTERNALCORE_API voxel_t GAccumulatedItemIndexStart;

/**
 * The null accumulated index. Defined as a macro as we cannot forward declare the struct and use the constructors
 * at the same time. See the Accumulated#Null below for the actual type definition.
 */
#define ACCUMULATED_PRIVATE_NULL_IDX ECommonVoxels::Null
/**
 * The null accumulated amount. Defined as a macro as we cannot forward declare the struct and use the constructors
 * at the same time. See the Accumulated#Null below for the actual type definition.
 */
#define ACCUMULATED_PRIVATE_NULL_AMT 0x0

/**
 * The maximum number of accumulated items that can be stored.
 */
#define ACCUMULATED_MAX_AMT 0xFFFF

USTRUCT()
struct JAFGEXTERNALCORE_API FAccumulated
{
    GENERATED_BODY()

    FORCEINLINE FAccumulated(void)
    {
        this->AccumulatedIndex = ACCUMULATED_PRIVATE_NULL_IDX;
        this->Amount           = ACCUMULATED_PRIVATE_NULL_AMT;

        return;
    }

    FORCEINLINE explicit FAccumulated(const voxel_t InAccumulatedIndex)
    {
        this->AccumulatedIndex = InAccumulatedIndex;
        this->Amount           = ACCUMULATED_PRIVATE_NULL_IDX == InAccumulatedIndex ? 0 : 1;

        return;
    }

    FORCEINLINE explicit FAccumulated(const voxel_t InAccumulatedIndex, const accamount_t InAmount)
    {
        this->AccumulatedIndex = InAccumulatedIndex;
        this->Amount           = InAmount;

        return;
    }

    UPROPERTY( /* Replicated */ )
    uint32 /* voxel_t */ AccumulatedIndex;
    UPROPERTY( /* Replicated */ )
    uint16 /* accamount_t */ Amount;

    /** Checks for overflow, underflow and if post add amount is zero, the Null Accumulated is set. */
    FORCEINLINE void SafeAddAmount(const accamount_t InAmount, bool& bCouldProcess)
    {
        if (InAmount == 0)
        {
            bCouldProcess = true;
            return;
        }

        int64 NewAmount  = this->Amount;
        NewAmount += InAmount;

        /* Underflow Error */
        if (NewAmount < 0)
        {
            bCouldProcess = false;
            return;
        }

        if (NewAmount == 0)
        {
            this->AccumulatedIndex = ACCUMULATED_PRIVATE_NULL_IDX;
            this->Amount           = ACCUMULATED_PRIVATE_NULL_AMT;
            bCouldProcess          = true;

            return;
        }

        /* Overflow Error */
        if (NewAmount > ACCUMULATED_MAX_AMT)
        {
            bCouldProcess = false;
            return;
        }

        this->Amount    += InAmount;
        bCouldProcess    = true;

        return;
    }

    /** Does not compare amount. */
    FORCEINLINE        auto operator==(const FAccumulated& O) const -> bool { return this->AccumulatedIndex == O.AccumulatedIndex; }
    /** Does not compare amount. */
    FORCEINLINE static auto Equals(const FAccumulated& A, const FAccumulated& B) -> bool { return A == B; }
    FORCEINLINE        auto IsVoxel(void) const -> bool { return this->AccumulatedIndex < GAccumulatedItemIndexStart; }
    FORCEINLINE        auto ToString(void) const -> FString
    {
        return FString::Printf(TEXT("FAccumulated{AccumulatedIndex:%d, Amount:%d}"), this->AccumulatedIndex, this->Amount);
    }
};

namespace Accumulated
{

static const FAccumulated Null { ACCUMULATED_PRIVATE_NULL_IDX, ACCUMULATED_PRIVATE_NULL_AMT };

}

#undef ACCUMULATED_PRIVATE_NULL_AMT
#undef ACCUMULATED_PRIVATE_NULL_IDX
