// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Accumulated.h"

#include "Slot.generated.h"

JAFG_VOID

class AWorldCharacter;

/**
 * A generic container slot implementation.
 * This class is only responsible for basic data exchange logic.
 *
 * No UI logic should be implemented here.
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

    auto OnPrimaryClicked(const AWorldCharacter* Owner, bool& bOutChangedData, const bool bCalledInServerRPC = false) -> void;

    auto OnDrop(const AWorldCharacter* Owner, bool& bOutChangedData, const bool bCalledInServerRPC = false) -> void;
};
