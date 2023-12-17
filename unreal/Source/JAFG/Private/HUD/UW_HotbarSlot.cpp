// © 2023 mzoesch. All rights reserved.

#include "HUD/UW_HotbarSlot.h"

#include "Components/Image.h"

void UUW_HotbarSlot::OnItemPreviewUpdate(const EItem Item)
{
    switch (Item)
    {
    case NullItem:
        this->ItemPreview->SetBrushFromTexture(nullptr);
        this->ItemPreview->SetColorAndOpacity(FLinearColor::Transparent);
        break;
    case StoneVoxel:
        this->ItemPreview->SetBrushFromTexture(this->TStoneVoxel);
        this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
        break;
    case DirtVoxel:
        this->ItemPreview->SetBrushFromTexture(this->TDirtVoxel);
        this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
        break;
    case GrassVoxel:
        this->ItemPreview->SetBrushFromTexture(this->TGrassVoxel);
        this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
        break;
    case GlassVoxel:
        this->ItemPreview->SetBrushFromTexture(this->TGlassVoxel);
        this->ItemPreview->SetColorAndOpacity(FLinearColor::White);
        break;
    default:
        this->ItemPreview->SetBrushFromTexture(nullptr);
        this->ItemPreview->SetColorAndOpacity(FLinearColor::Transparent);
        UE_LOG(LogTemp, Error, TEXT("UUW_HotbarSlot::OnItemPreviewUpdate: Item not found: %s."), *UEnum::GetValueAsString(Item));
        break;
    }

    return;
}
