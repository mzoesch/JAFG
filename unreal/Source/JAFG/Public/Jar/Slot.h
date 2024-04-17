// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "Accumulated.h"
#include "CommonCore.h"

#include "Slot.generated.h"

JAFG_VOID

/**
 * A generic container slot implementation.
 * This class is only responsible for basic data exchange logic.
 */
USTRUCT()
struct JAFG_API FSlot
{
    GENERATED_BODY()

    FSlot(void) = default;
    explicit FSlot(const FAccumulated& Content) : Content(Content)
    {
    }
    virtual ~FSlot(void) = default;

    UPROPERTY()
    FAccumulated Content;
};
