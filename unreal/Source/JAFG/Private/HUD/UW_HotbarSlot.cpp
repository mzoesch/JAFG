// © 2023 mzoesch. All rights reserved.

#include "HUD/UW_HotbarSlot.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UUW_HotbarSlot::OnItemPreviewUpdate(const FAccumulated Accumulated) const
{
    if (Accumulated.GetVoxel() != EVoxel::Null)
    {
        this->ItemAmount->SetText(FText::FromString(FString::FromInt(Accumulated.GetAmount())));
        
        switch (Accumulated.GetVoxel())
        {
        case EVoxel::Null:
            this->ItemPreview->SetBrushFromTexture(nullptr);
            this->ItemPreview->SetColorAndOpacity(FLinearColor::Transparent);
            return;
        case EVoxel::Air:
            UE_LOG(LogTemp, Error, TEXT("UUW_HotbarSlot::OnItemPreviewUpdate: Air voxel should not be in hotbar."));
            return;
        case EVoxel::Stone:
            this->ItemPreview->SetBrushFromTexture(this->TStoneVoxel);
            this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
            return;
        case EVoxel::Dirt:
            this->ItemPreview->SetBrushFromTexture(this->TDirtVoxel);
            this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
            return;
        case EVoxel::Grass:
            this->ItemPreview->SetBrushFromTexture(this->TGrassVoxel);
            this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
            return;
        case EVoxel::Glass:
            this->ItemPreview->SetBrushFromTexture(this->TGlassVoxel);
            this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
            return;
        case EVoxel::Log:
            this->ItemPreview->SetBrushFromTexture(this->TLogVoxel);
            this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
            return;
        case EVoxel::Planks:
            this->ItemPreview->SetBrushFromTexture(this->TPlanksVoxel);
            this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
            return;
        case EVoxel::Leaves:
            this->ItemPreview->SetBrushFromTexture(this->TLeavesVoxel);
            this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
            return;
        default:
            this->ItemPreview->SetBrushFromTexture(nullptr);
            this->ItemPreview->SetColorAndOpacity(FLinearColor::Transparent);
            UE_LOG(LogTemp, Error, TEXT("UUW_HotbarSlot::OnItemPreviewUpdate: Item not found: %s."), *UEnum::GetValueAsString(Accumulated.GetItem()));
            return;
        }
    }

    if (Accumulated.GetItem() != EItem::NullItem)
    {
        this->ItemAmount->SetText(FText::FromString(FString::FromInt(Accumulated.GetAmount())));
        UE_LOG(LogTemp, Error, TEXT("UUW_HotbarSlot::OnItemPreviewUpdate: Item not found: %s."), *UEnum::GetValueAsString(Accumulated.GetItem()));
        return;
    }

    this->ItemAmount->SetText(FText());
    this->ItemPreview->SetBrushFromTexture(nullptr);
    this->ItemPreview->SetColorAndOpacity(FLinearColor::Transparent);
    
    return;
}
