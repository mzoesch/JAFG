// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Lib/FAccumulated.h"

#include "ContainerSlotData.generated.h"

/** Every container slot should use this or a derived class to store its data. */
UCLASS()
class UContainerSlotData : public UObject
{
    GENERATED_BODY()
    
public:

    UPROPERTY(BlueprintReadOnly)
    int32           Index;
    FAccumulated    Accumulated;
    
};
