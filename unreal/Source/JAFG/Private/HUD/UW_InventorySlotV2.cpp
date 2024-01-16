// © 2023 mzoesch. All rights reserved.

#include "HUD/UW_InventorySlotV2.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Assets/General.h"
#include "World/WorldVoxel.h"
#include "Core/GI_Master.h"
#include "HUD/InventorySlotData.h"

#define GI Cast<UGI_Master>(this->GetGameInstance())

void UUW_InventorySlotV2::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    UInventorySlotData* InventorySlotData = Cast<UInventorySlotData>(ListItemObject);

    if (InventorySlotData == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UUW_InventorySlotV2::NativeOnListItemObjectSet: InventorySlotData is null."));
        return;
    }

    this->ItemAmount->SetText(InventorySlotData->Amount != 0 ? FText::FromString(FString::FromInt(InventorySlotData->Amount)) : FText());
    
}

void UUW_InventorySlotV2::OnItemPreviewUpdate(const FAccumulated Accumulated) const
{
    this->ItemAmount->SetText(Accumulated.GetAmount() != 0 ? FText::FromString(FString::FromInt(Accumulated.GetAmount())) : FText());
    
    if (Accumulated.GetVoxel() != EWorldVoxel::VoxelNull)
    {
        if (UTexture2D* Texture = FGeneral::LoadTexture2D(Accumulated))
        {
            this->ItemPreview->SetBrushFromTexture(Texture);
            this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
            return;
        }

        this->ItemPreview->SetBrushFromTexture(GI->NoTexture);
        this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
        return;
    }

    if (Accumulated.GetItem() != EItem::NullItem)
    {
        UE_LOG(LogTemp, Error, TEXT("UUW_HotbarSlot::OnItemPreviewUpdate: Item not found: %s."), *UEnum::GetValueAsString(Accumulated.GetItem()));
        return;
    }

    this->ItemAmount->SetText(FText());
    this->ItemPreview->SetBrushFromTexture(nullptr);
    this->ItemPreview->SetColorAndOpacity(FLinearColor::Transparent);
    
    return;
}

#undef GI
