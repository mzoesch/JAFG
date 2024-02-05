// © 2023 mzoesch. All rights reserved.

#include "HUD/UW_AccumulatedCursorPreview.h"

#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/UserInterfaceSettings.h"

#include "Core/CH_Master.h"
#include "Assets/General.h"
#include "Components/CanvasPanelSlot.h"
#include "Core/GI_Master.h"
#include "World/WorldVoxel.h"

#define UIL_LOG(Verbosity, Format, ...)     UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define GI                                  Cast<UGI_Master>(this->GetGameInstance())
#define CH                                  Cast<ACH_Master>(this->GetOwningPlayerPawn())

void UUW_AccumulatedCursorPreview::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    this->SetVisibility(ESlateVisibility::HitTestInvisible);
    
    this->OnUpdate();

    return;
}

void UUW_AccumulatedCursorPreview::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(this->AccumulatedPreview->Slot);
    
    FVector2D ViewportSize;
    this->GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
    
    const float DPIScaling = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));
    
    FVector2D MousePosition;
    if (!this->GetOwningPlayer()->GetMousePosition(MousePosition.X, MousePosition.Y))
    {
        return;
    }

    CanvasPanelSlot->SetPosition(MousePosition / DPIScaling - FVector2D(25.0f, 25.0f));
    
    return;
}

void UUW_AccumulatedCursorPreview::OnUpdate()
{
    if (CH->GetHandAccumulated().GetVoxel() != EWorldVoxel::VoxelNull)
    {
        if (UTexture2D* Texture = FGeneral::LoadTexture2D(CH->GetHandAccumulated()))
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

#undef UIL_LOG
#undef GI
#undef CH
