// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "World/Voxel.h"
#include "Lib/Item.h"

#include "FAccumulated.generated.h"

USTRUCT()
struct JAFG_API FAccumulated
{
    GENERATED_BODY()

public:

    FAccumulated();
    explicit FAccumulated(const EVoxel V);
    explicit FAccumulated(const EItem I);
    explicit FAccumulated(const EVoxel V, const uint16_t);
    explicit FAccumulated(const EItem I, const uint16_t);
    explicit FAccumulated(const EItem I, const EVoxel V);
    explicit FAccumulated(const EVoxel V, const EItem I, const uint16_t A);

private:

    EVoxel Voxel;
    EItem Item;
    uint16_t Amount;

public:

    static const FAccumulated NullAccumulated;
    
public:
    
    EVoxel GetVoxel() const { return this->Voxel; }
    EItem GetItem() const { return this->Item; }
    uint16_t GetAmount() const { return this->Amount; }
    void SetAmount(const uint16_t A) { this->Amount = A; }
    void AddAmount(const uint16_t A) { this->Amount += A; }

public:
    
    /** Does not compare the amount. */
    inline bool operator==(const FAccumulated& Accumulated) const { return this->Voxel == Accumulated.Voxel && this->Item == Accumulated.Item; }
    /** Does not compare the amount. */
    static inline bool Equals(const FAccumulated& A, const FAccumulated& B) { return A == B; }
    FString ToString() const;

};
