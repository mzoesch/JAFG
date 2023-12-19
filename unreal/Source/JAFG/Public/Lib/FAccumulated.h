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

private:

    EVoxel Voxel;
    EItem Item;
    unsigned __int32 Amount;

public:
    
    EVoxel GetVoxel() const { return this->Voxel; }
    EItem GetItem() const { return this->Item; }

};
