#pragma once

#include "CoreMinimal.h"

#include "World/Voxel.h"
#include "Lib/Item.h"

#include "FCollectable.generated.h"

USTRUCT()
struct JAFG_API FCollectable
{
    GENERATED_BODY()
    
public:

    FCollectable() = default;
    explicit FCollectable(const EVoxel Voxel);
    explicit FCollectable(const EItem Item);

private:

    
    
};
