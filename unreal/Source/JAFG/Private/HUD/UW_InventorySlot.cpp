// © 2023 mzoesch. All rights reserved.

#include "HUD/UW_InventorySlot.h"

#include "Components/Image.h"

#include "Core/GI_Master.h"
#include "Core/CH_Master.h"
#include "HUD/InventorySlotData.h"
#include "World/WorldVoxel.h"
#include "Assets/General.h"

#define GI Cast<UGI_Master>(this->GetGameInstance())
#define CH Cast<ACH_Master>(this->GetOwningPlayerPawn())

void UUW_InventorySlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    this->InventorySlotData = Cast<UInventorySlotData>(ListItemObject);
    
    this->SlotBackground->SetColorAndOpacity(FLinearColor(0.1f, 0.1f, 0.1f, 1.0f));

    if (this->InventorySlotData->Accumulated.GetVoxel() != EWorldVoxel::VoxelNull)
    {
        if (UTexture2D* Texture = FGeneral::LoadTexture2D(this->InventorySlotData->Accumulated))
        {
            this->AccumulatedPreview->SetBrushFromTexture(Texture);
            this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::White);

            return;
        }
        
        this->AccumulatedPreview->SetBrushFromTexture(GI->NoTexture);
        this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::White);
        
        return;
    }
    
    this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::Transparent);
    
    return;
}

void UUW_InventorySlot::OnClick()
{
    CH->OnInventorySlotClick(this->InventorySlotData->Index);
}

#undef GI
#undef CH
