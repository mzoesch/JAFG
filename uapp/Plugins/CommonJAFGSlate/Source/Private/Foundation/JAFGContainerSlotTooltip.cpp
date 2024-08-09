// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/JAFGContainerSlotTooltip.h"
#include "Components/JAFGTextBlock.h"
#include "System/VoxelSubsystem.h"

UJAFGContainerSlotTooltip::UJAFGContainerSlotTooltip(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGContainerSlotTooltip::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FSlotTooltipPassData)
    {
        this->RepresentedSlot = Data->Slot;
    }

    this->MarkAsDirtySynchronous();

    return;
}

FString UJAFGContainerSlotTooltip::GetAccumulatedName(void) const
{
    if (this->RepresentedSlot == nullptr)
    {
        return FString();
    }

    return this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>()
        ->GetAccumulatedName(this->RepresentedSlot->GetSlotData()->GetSlotValueRef().AccumulatedIndex);
}

FString UJAFGContainerSlotTooltip::GetAccumulatedNamespace(void) const
{
    return this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>()
        ->GetAccumulatedNamespace(this->RepresentedSlot->GetSlotData()->GetSlotValueRef().AccumulatedIndex);
}

FString UJAFGContainerSlotTooltip::GetAccumulatedIndex(void) const
{
    return FString::FromInt(this->RepresentedSlot->GetSlotData()->GetSlotValueRef().AccumulatedIndex);
}

void UJAFGContainerSlotTooltip::OnRefresh(void)
{
    if (this->TextBlock_AccumulatedName)
    {
        this->TextBlock_AccumulatedName->SetText(FText::FromString(this->GetAccumulatedName()));
    }

    if (this->TextBlock_AccumulatedDebugInfo)
    {
        this->TextBlock_AccumulatedDebugInfo->SetText(FText::FromString(
            FString::Printf(TEXT("%s::%s [%s]"),
                *this->GetAccumulatedNamespace(),
                *this->GetAccumulatedName(),
                *this->GetAccumulatedIndex()
            )
        ));
    }

    if (this->TextBlock_AccumulatedDebugName)
    {
        this->TextBlock_AccumulatedDebugName->SetText(FText::FromString(
            FString::Printf(TEXT("%s::%s"),
                *this->GetAccumulatedNamespace(),
                *this->GetAccumulatedName()
            )
        ));
    }

    if (this->TextBlock_AccumulatedIndex)
    {
        this->TextBlock_AccumulatedDebugName->SetText(FText::FromString(
            FString::Printf(TEXT("%s"),
                *this->GetAccumulatedIndex()
            )
        ));
    }

    return;
}
