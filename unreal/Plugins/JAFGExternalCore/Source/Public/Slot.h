// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Accumulated.h"

#include "Slot.generated.h"

class IContainerOwner;

USTRUCT(NotBlueprintType)
struct JAFGEXTERNALCORE_API FSlot
{
    GENERATED_BODY()

    FSlot(void) = default;
    explicit FSlot(const FAccumulated& Accumulated) : Content(Accumulated) { }

    FAccumulated Content;

    /** @return If data was changed. */
    bool OnPrimaryClicked(IContainerOwner* Owner);
    /** @return If data was changed. */
    bool OnSecondaryClicked(IContainerOwner* Owner);

    static bool AddToFirstSuitableSlot(TArray<FSlot>& Container, const FAccumulated& Value);
};
