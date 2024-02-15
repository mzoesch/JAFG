// © 2023 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Lib/FAccumulated.h"

#include "Slot.generated.h"

class ACH_Master;

/**
 * A generic container slot implementation.
 *
 * This is not a UWidget implementation, therefore no HUD elements are associated with this class.
 * All HUD Actions must be handled by the derived class.
 * This class is only responsible for data exchange logic.
 */
USTRUCT()
struct JAFG_API FSlot
{
    GENERATED_BODY()

public:

    FSlot() = default;
    explicit FSlot(const FAccumulated& Content) : Content(Content) { }
    
    FAccumulated Content;

    /** It is always assumed that the slot can be accessed by the local player. */
    void OnClicked(ACH_Master* Trigger, bool& bOutChangedData);
    /** It is always assumed that the slot can be accessed by the local player. */
    void OnSecondaryClicked(ACH_Master* Trigger, bool& bOutChangedData);
};
