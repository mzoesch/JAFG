// Copyright 2024 mzoesch. All rights reserved.

#include "UI/OSD/Encyclopedia/EncyclopediaTabBarPanel_Accumulated.h"
#include "Components/TileView.h"
#include "Foundation/JAFGContainerSlot.h"
#include "System/VoxelSubsystem.h"

UEncyclopediaTabBarPanel_Accumulated::UEncyclopediaTabBarPanel_Accumulated(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UEncyclopediaTabBarPanel_Accumulated::NativeConstruct(void)
{
    Super::NativeConstruct();

    const UVoxelSubsystem* VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    for (voxel_t_signed i = 0; i < VoxelSubsystem->GetAccumulatedNum_Signed(); i++)
    {
        UJAFGReadOnlyContainerSlotData* SlotData = NewObject<UJAFGReadOnlyContainerSlotData>(this);
        SlotData->Content = FAccumulated(i);

        this->TileView_Accumulated->AddItem(SlotData);

        continue;
    }

    return;
}
