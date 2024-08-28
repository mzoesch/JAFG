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

USTRUCT(NotBlueprintable, NotBlueprintType)
struct JAFGEXTERNALCORE_API FAccumulated
{
    GENERATED_BODY()

    //////////////////////////////////////////////////////////////////////////
    // Constructors
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

    /**
     * This is a slow operation, use it with caution, e.g., do not use every frame.
     * @note Null-Accumulated are not allowed to be constructed with this constructor.
     */
                explicit FAccumulated(const UObject& Context, const FString& InAccumulatedName);
    FORCEINLINE explicit FAccumulated(const UObject* const Context, const FString& InAccumulatedName)
                : FAccumulated(*Context, InAccumulatedName) { return; }

    /**
     * This is a slow operation, use it with caution, e.g., do not use every frame.
     * @note Null-Accumulated are not allowed to be constructed with this constructor.
     */
                explicit FAccumulated(const UObject& Context, const FString& InAccumulatedNamespace, const FString& InAccumulatedName);
    FORCEINLINE explicit FAccumulated(const UObject* const Context, const FString& InAccumulatedNamespace, const FString& InAccumulatedName)
                : FAccumulated(*Context, InAccumulatedNamespace, InAccumulatedName) { return; }

    /**
     * This is a slow operation, use it with caution, e.g., do not use every frame.
     * @note Null-Accumulated are not allowed to be constructed with this constructor.
     */
                explicit FAccumulated(const UObject& Context, const FString& InAccumulatedName, const accamount_t InAmount);
    FORCEINLINE explicit FAccumulated(const UObject* const Context, const FString& InAccumulatedName, const accamount_t InAmount)
                : FAccumulated(*Context, InAccumulatedName, InAmount) { return; }

    /**
     * This is a slow operation, use it with caution, e.g., do not use every frame.
     * @note Null-Accumulated are not allowed to be constructed with this constructor.
     */
                explicit FAccumulated(const UObject& Context, const FString& InAccumulatedNamespace, const FString& InAccumulatedName, const accamount_t InAmount);
    FORCEINLINE explicit FAccumulated(const UObject* const Context, const FString& InAccumulatedNamespace, const FString& InAccumulatedName, const accamount_t InAmount)
                : FAccumulated(*Context, InAccumulatedNamespace, InAccumulatedName, InAmount) { return; }
    // ~Constructors
    //////////////////////////////////////////////////////////////////////////

    UPROPERTY( /* Replicated */ )
    uint32 /* voxel_t */ AccumulatedIndex;
    UPROPERTY( /* Replicated */ )
    uint16 /* accamount_t */ Amount;

    /** Checks for overflow, underflow and if post add amount is zero, the Null Accumulated is set. */
    FORCEINLINE void SafeAddAmount(const accamount_t_signed InAmount, bool& bOutCouldProcess)
    {
        if (InAmount == 0)
        {
            bOutCouldProcess = true;
            return;
        }

        int64 NewAmount  = this->Amount;
        NewAmount += InAmount;

        /* Underflow Error */
        if (NewAmount < 0)
        {
            bOutCouldProcess = false;
            return;
        }

        if (NewAmount == 0)
        {
            this->AccumulatedIndex = ACCUMULATED_PRIVATE_NULL_IDX;
            this->Amount           = ACCUMULATED_PRIVATE_NULL_AMT;
            bOutCouldProcess          = true;

            return;
        }

        /* Overflow Error */
        if (NewAmount > ACCUMULATED_MAX_AMT)
        {
            bOutCouldProcess = false;
            return;
        }

        this->Amount    += InAmount;
        bOutCouldProcess    = true;

        return;
    }
    FORCEINLINE bool SafeAddAmountRet(const accamount_t_signed InAmount)
    {
        bool bCouldProcess = false;
        this->SafeAddAmount(InAmount, bCouldProcess);

        return bCouldProcess;
    }
    FORCEINLINE void SafeAddAmount(const accamount_t_signed InAmount)
    {
        bool bCouldProcess = false;
        this->SafeAddAmount(InAmount, bCouldProcess);

        if (bCouldProcess == false)
        {
            LOG_FATAL(
                LogContainerStuff,
                "Detected an %s error while adding [%d] amount to [%s].",
                InAmount < 0 ? TEXT("underflow") : TEXT("overflow"), InAmount, *this->ToString()
            )
        }

        return;
    }

    /** Meaningless if the amount is zero. */
    FORCEINLINE auto GetHalfAmount(void) const -> int32 { return this->Amount / 2 == 0 ? 1 : this->Amount / 2; }
    FORCEINLINE void SubtractHalfAmount(void) { this->SafeAddAmount(-this->GetHalfAmount()); }

    /** Does not compare amount. */
    FORCEINLINE        auto operator==(const FAccumulated& O) const -> bool { return this->AccumulatedIndex == O.AccumulatedIndex; }
    FORCEINLINE        auto operator!=(const FAccumulated& O) const -> bool { return !(*this == O); }
    FORCEINLINE        auto operator==(const ECommonVoxels::Type CommonVoxels) const -> bool { return this->AccumulatedIndex == CommonVoxels; }
    FORCEINLINE        auto operator!=(const ECommonVoxels::Type CommonVoxels) const -> bool { return !(*this == CommonVoxels); }

    /** Does compare amount. */
    FORCEINLINE static auto Equals(const FAccumulated& A, const FAccumulated& B) -> bool { return A == B; }
    FORCEINLINE static auto DeepEquals(const FAccumulated& A, const FAccumulated& B) -> bool { return A == B && A.Amount == B.Amount; }

    FORCEINLINE        auto IsNull(void) const -> bool { return *this == FAccumulated(ACCUMULATED_PRIVATE_NULL_IDX, ACCUMULATED_PRIVATE_NULL_AMT); }
    FORCEINLINE        auto IsAir(void) const -> bool { return *this == ECommonVoxels::Air; }

    FORCEINLINE        auto IsVoxel(void) const -> bool { return this->AccumulatedIndex < GAccumulatedItemIndexStart; }
    FORCEINLINE static auto IsVoxel(const FAccumulated& A) -> bool { return A.IsVoxel(); }
    FORCEINLINE static auto IsVoxel(const voxel_t InAccumulatedIndex) -> bool { return InAccumulatedIndex < GAccumulatedItemIndexStart; }

    FORCEINLINE        auto ToString(void) const -> FString
    {
        return FString::Printf(TEXT("FAccumulated{AccumulatedIndex:%d, Amount:%d}"), this->AccumulatedIndex, this->Amount);
    }
    FORCEINLINE        auto ToShortString(void) const -> FString
    {
        return FString::Printf(TEXT("{%d,%d}"), this->AccumulatedIndex, this->Amount);
    }

    //////////////////////////////////////////////////////////////////////////
    // Slow operations
    //////////////////////////////////////////////////////////////////////////

    auto GetDisplayName(const UObject* const Context) const -> FString;
};

namespace Accumulated
{

static const FString NamespaceToNameSplitter { TEXT("::") };

static const FAccumulated Null { ACCUMULATED_PRIVATE_NULL_IDX, ACCUMULATED_PRIVATE_NULL_AMT };

/**
 * Try to split an accumulated name into its namespace and name.
 *     Space::Name  => (   true ) Space, Name
 *     Space::      => (  false )
 *     ::Name       => (  false )
 *     Name         => (  false )
 *
 * @return True, if OutNamespace and OutName is meaningful.
 */
FORCEINLINE JAFGEXTERNALCORE_API bool Split(const FString& AccumulatedName, FString& OutNamespace, FString& OutName)
{
    if (AccumulatedName.Split(Accumulated::NamespaceToNameSplitter, &OutNamespace, &OutName) == false)
    {
        OutNamespace.Empty();
        OutName.Empty();
        return false;
    }

    if (OutNamespace.IsEmpty() || OutName.IsEmpty())
    {
        OutNamespace.Empty();
        OutName.Empty();
        return false;
    }

    return true;
}

FORCEINLINE JAFGEXTERNALCORE_API bool UnrealSplit(const FString& AccumulatedName, FString& OutNamespace, FString& OutName)
{
    if (AccumulatedName.Split(Accumulated::NamespaceToNameSplitter, &OutNamespace, &OutName) == false)
    {
        OutNamespace.Empty();
        OutName.Empty();
        return false;
    }

    return true;
}

FORCEINLINE JAFGEXTERNALCORE_API FString Join(const FString& Namespace, const FString& Name)
{
    return Namespace + Accumulated::NamespaceToNameSplitter + Name;
}

}

#undef ACCUMULATED_PRIVATE_NULL_AMT
#undef ACCUMULATED_PRIVATE_NULL_IDX
