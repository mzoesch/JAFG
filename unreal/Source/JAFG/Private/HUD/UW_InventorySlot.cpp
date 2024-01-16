// © 2023 mzoesch. All rights reserved.


#include "HUD/UW_InventorySlot.h"

#include "Components/Image.h"

#include "Assets/General.h"
#include "World/WorldVoxel.h"
#include "Core/GI_Master.h"

#define GI Cast<UGI_Master>(this->GetGameInstance())

void UUW_InventorySlot::OnAccumulatedPreviewUpdate(const FAccumulated Accumulated) const
{

    // if (Accumulated.GetVoxel() != EWorldVoxel::VoxelNull)
    // {
    //     if (UTexture2D* Texture = FGeneral::LoadTexture2D(Accumulated))
    //     {
    //         this->AccumulatedPreview->SetBrushFromTexture(Texture);
    //         this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::White);
    //         return;
    //     }
    //     
    //     this->AccumulatedPreview->SetBrushFromTexture(GI->NoTexture);
    //     this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::White);
    //     return;
    // }
    
    UE_LOG(LogTemp, Warning, TEXT("UUW_InventorySlot::OnAccumulatedPreviewUpdate: V: %d)"), Accumulated.GetVoxel());

    this->AccumulatedPreview->SetBrushFromTexture(GI->NoTexture);
    this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::White);
    
    return;
}

#undef GI
