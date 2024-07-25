// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/PlayerInventory.h"
#include "WorldCore/Character/CharacterCrafterComponent.h"

UPlayerInventory::UPlayerInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UPlayerInventory::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->SetAutoUnsubscribeOtherContainerOnKill(false);

    this->SetOtherContainerDisplayName("UsedDisplayName");
    this->SetOtherContainerData(this->GetOwningPlayerPawn()->GetComponentByClass<UCharacterCrafterComponent>()->AsContainerCrafter());
    this->NowDoBuildDeferred();

    return;
}
