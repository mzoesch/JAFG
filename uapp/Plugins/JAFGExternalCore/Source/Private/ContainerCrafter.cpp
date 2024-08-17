// Copyright 2024 mzoesch. All rights reserved.

#include "ContainerCrafter.h"

void IContainerCrafter::ReduceContainerByOne(void)
{
    this->ReduceContainerBy(1);
}

void IContainerCrafter::ReduceContainerBy(const int32 InAmount)
{
    for (FSlot& Slot : this->GetContainer())
    {
        if (Slot.Content.IsNull())
        {
            continue;
        }

        Slot.Content.SafeAddAmount(-InAmount);

        continue;
    }

    return;
}
