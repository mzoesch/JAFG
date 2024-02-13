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
    explicit FAccumulated(const int InAccumulated);
    explicit FAccumulated(const int InAccumulated, const int InAmount);

private:

    int         Accumulated;
    uint16_t    Amount;

public:

    static const FAccumulated NullAccumulated;
    
public:
    
    int GetVoxel() const { return this->Accumulated; }
    void SetVoxel(const int V) { this->Accumulated = V; }
    uint16_t GetAmount() const { return this->Amount; }
    void SetAmount(const uint16_t A) { this->Amount = A; }
    void AddAmount(const uint16_t A) { this->Amount += A; }

public:
    
    /** Does not compare the amount. */
    FORCEINLINE bool operator==(const FAccumulated& O) const { return this->Accumulated == O.Accumulated; }
    /** Does not compare the amount. */
    static FORCEINLINE bool Equals(const FAccumulated& A, const FAccumulated& B) { return A == B; }
    FString ToString() const;
};
