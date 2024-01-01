// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Lib/Item.h"

#include "FAccumulated.generated.h"

USTRUCT()
struct JAFG_API FAccumulated
{
    GENERATED_BODY()

public:

    FAccumulated();
    explicit FAccumulated(const int V);
    explicit FAccumulated(const EItem I);
    explicit FAccumulated(const int V, const uint16_t);
    explicit FAccumulated(const EItem I, const uint16_t);
    explicit FAccumulated(const EItem I, const int V);
    explicit FAccumulated(const int V, const EItem I, const uint16_t A);

private:

    int Voxel;
    EItem Item;
    uint16_t Amount;

public:

    static const FAccumulated NullAccumulated;
    
public:
    
    int GetVoxel() const { return this->Voxel; }
    EItem GetItem() const { return this->Item; }
    uint16_t GetAmount() const { return this->Amount; }
    void SetAmount(const uint16_t A) { this->Amount = A; }
    void AddAmount(const uint16_t A) { this->Amount += A; }

public:
    
    /** Does not compare the amount. */
    inline bool operator==(const FAccumulated& O) const { return this->Voxel == O.Voxel && this->Item == O.Item; }
    /** Does not compare the amount. */
    static inline bool Equals(const FAccumulated& A, const FAccumulated& B) { return A == B; }
    FString ToString() const;
};
