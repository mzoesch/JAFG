// © 2023 mzoesch. All rights reserved.

#include "HUD/Container/Slots/ContainerSlot.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Assets/General.h"
#include "Lib/HUD/Container/ContainerSlotData.h"
#include "World/WorldVoxel.h"
#include "Core/GI_Master.h"

#define GI  Cast<UGI_Master>(this->GetGameInstance())
#define CH  Cast<ACH_Master>(this->GetOwningPlayerPawn())

void UW_ContainerSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    this->ContainerSlotData = Cast<UContainerSlotData>(ListItemObject);

    this->RenderSlot();
    
    return;
}

void UW_ContainerSlot::RenderSlot()
{
    this->SlotBackground->SetColorAndOpacity(FLinearColor(0.1f, 0.1f, 0.1f, 1.0f));

    if (this->ContainerSlotData->Accumulated.GetVoxel() != EWorldVoxel::VoxelNull)
    {
        this->AccumulatedAmount->SetText(this->ContainerSlotData->Accumulated.GetAmount() != 0 ? FText::FromString(FString::FromInt(this->ContainerSlotData->Accumulated.GetAmount())) : FText());

        if (UTexture2D* Texture = FGeneral::LoadTexture2D(this->ContainerSlotData->Accumulated))
        {
            this->AccumulatedPreview->SetBrushFromTexture(Texture);
            this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::White);

            return;
        }

        this->AccumulatedPreview->SetBrushFromTexture(GI->NoTexture);
        this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::White);
        
        return;
    }

    this->AccumulatedAmount->SetText(FText());
    this->AccumulatedPreview->SetBrushFromTexture(nullptr);
    this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::Transparent);

    return;
}

#undef GI
#undef CH
