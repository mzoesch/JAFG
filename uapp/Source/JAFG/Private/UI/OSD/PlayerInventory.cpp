// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/PlayerInventory.h"
#include "Foundation/JAFGContainerCrafterSlot.h"
#include "WorldCore/Character/CharacterCrafterComponent.h"

UPlayerInventory::UPlayerInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetAutoUnsubscribeOtherContainerOnKill(false);
    return;
}

void UPlayerInventory::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->SetOtherContainerDisplayName("UsedDisplayName");
    this->SetOtherContainerData(this->GetOwningPlayerPawn()->GetComponentByClass<UCharacterCrafterComponent>()->AsContainerCrafter());

    if (this->GetOtherContainerAsCrafter() == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Other container is not a container crafter.")
        return;
    }

    this->NowDoBuildDeferred();

    this->Slot_InventoryCrafterProduct->SetCrafterTargetContainer(this->GetOtherContainerAsCrafter());

    return;
}

IContainerCrafter* UPlayerInventory::GetOtherContainerAsCrafter(void) const
{
    return Cast<IContainerCrafter>(this->OtherContainerData);
}
