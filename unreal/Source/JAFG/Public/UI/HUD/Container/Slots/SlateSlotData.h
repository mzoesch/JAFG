// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Jar/Accumulated.h"

#include "SlateSlotData.generated.h"

JAFG_VOID

/** Every container slot should use this or a derived class to store its data. */
UCLASS(NotBlueprintable)
class JAFG_API USlateSlotData : public UObject
{
    GENERATED_BODY()

public:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32        Index;
    FAccumulated Accumulated;
    // UPROPERTY()
    // TObjectPtr<UW_Container> OuterContainer;
};
