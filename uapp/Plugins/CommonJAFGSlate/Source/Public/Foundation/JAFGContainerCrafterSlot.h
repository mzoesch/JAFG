// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGContainerSlot.h"

#include "JAFGContainerCrafterSlot.generated.h"

class IContainerCrafter;

/** Optional widget to use as a foundation for container slots that are represented a container crafter. */
UCLASS(Abstract, Blueprintable)
class COMMONJAFGSLATE_API UJAFGContainerCrafterSlot : public UJAFGContainerSlot
{
    GENERATED_BODY()

public:

    explicit UJAFGContainerCrafterSlot(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void SetCrafterTargetContainer(IContainerCrafter* TargetContainer);

private:

    IContainerCrafter* CrafterTargetContainer = nullptr;
    FDelegateHandle OnContainerChangedDelegateHandle;

    UFUNCTION()
    void OnCrafterTargetContainerChanged(const ELocalContainerChange::Type InReason, const int32 InIndex);
};
