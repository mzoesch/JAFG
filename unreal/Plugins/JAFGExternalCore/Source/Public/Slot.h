// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Accumulated.h"

#include "Slot.generated.h"

class IContainer;
class IContainerOwner;

USTRUCT(NotBlueprintable, NotBlueprintType)
struct JAFGEXTERNALCORE_API FSlot
{
    GENERATED_BODY()

    FSlot(void) = default;
    explicit FSlot(const FAccumulated& Accumulated) : Content(Accumulated) { }

    UPROPERTY( /* Replicated */ )
    FAccumulated Content;

    /** @return True, if data was changed. */
    bool OnPrimaryClicked(IContainerOwner* Owner);
    /** @return True, if data was changed. */
    bool OnSecondaryClicked(IContainerOwner* Owner);

    static bool AddToFirstSuitableSlot(TArray<FSlot>& Container, const FAccumulated& Value);

    FORCEINLINE        bool operator==(const FSlot& O) const { return this->Content == O.Content; }
    FORCEINLINE static bool Equals(const FSlot& A, const FSlot& B) { return A == B; }
    FORCEINLINE static bool DeepEquals(const FSlot& A, const FSlot& B) { return FAccumulated::DeepEquals(A.Content, B.Content); }
};
