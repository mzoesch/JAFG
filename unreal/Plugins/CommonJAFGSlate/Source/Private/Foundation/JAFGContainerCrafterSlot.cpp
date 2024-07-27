// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/JAFGContainerCrafterSlot.h"
#include "ContainerCrafter.h"

UJAFGContainerCrafterSlot::UJAFGContainerCrafterSlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGContainerCrafterSlot::SetCrafterTargetContainer(IContainerCrafter* TargetContainer)
{
    if (this->CrafterTargetContainer && this->OnContainerChangedDelegateHandle.IsValid())
    {
        if (this->CrafterTargetContainer->OnContainerChangedDelegate.Remove(this->OnContainerChangedDelegateHandle) == false)
        {
            LOG_ERROR(LogCommonSlate, "Failed to remove container changed delegate.")
        }
    }

    this->CrafterTargetContainer = TargetContainer;
    this->CrafterTargetContainer->OnContainerChangedDelegate.AddUObject(this, &UJAFGContainerCrafterSlot::OnCrafterTargetContainerChanged);

    return;
}

void UJAFGContainerCrafterSlot::OnCrafterTargetContainerChanged(const ELocalContainerChange::Type InReason, const int32 InIndex)
{
    LOG_WARNING(LogCommonSlate, "Crafter target container changed.")
}
