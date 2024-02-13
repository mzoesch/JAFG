// © 2023 mzoesch. All rights reserved.

#include "HUD/Assets/AccumulatedCursorPreview.h"

#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/UserInterfaceSettings.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "Core/CH_Master.h"
#include "Assets/General.h"
#include "Core/GI_Master.h"
#include "World/WorldVoxel.h"

#define UIL_LOG(Verbosity, Format, ...)     UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)
#define GI                                  Cast<UGI_Master>(this->GetGameInstance())
#define CH                                  Cast<ACH_Master>(this->GetOwningPlayerPawn())

void UW_AccumulatedCursorPreview::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    this->SetVisibility(ESlateVisibility::HitTestInvisible);

    this->AccumulatedWrapperSlot = Cast<UCanvasPanelSlot>(this->AccumulatedWrapper->Slot);
    
    this->OnUpdate();

    return;
}

void UW_AccumulatedCursorPreview::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    FVector2D ViewportSize;
    this->GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
    
    const float DPIScaling = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));
    
    FVector2D MousePosition;
    if (!this->GetOwningPlayer()->GetMousePosition(MousePosition.X, MousePosition.Y))
    {
        return;
    }

    this->AccumulatedWrapperSlot->SetPosition(MousePosition / DPIScaling - FVector2D(25.0f, 25.0f));
    
    return;
}

void UW_AccumulatedCursorPreview::OnUpdate()
{
    if (CH->CursorHand.GetVoxel() != EWorldVoxel::VoxelNull)
    {
        this->AccumulatedAmount->SetText(CH->CursorHand.GetAmount() != 0 ? FText::FromString(FString::FromInt(CH->CursorHand.GetAmount())) : FText());
        
        if (UTexture2D* Texture = FGeneral::LoadTexture2D(CH->CursorHand))
        {
            this->AccumulatedPreview->SetBrushFromTexture(Texture);
            this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::White);
            
            return;
        }

        this->AccumulatedPreview->SetBrushFromTexture(GI->NoTexture);
        this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::White);
        
        return;
    }

    UE_LOG(LogTemp, Error, TEXT("UUW_AccumulatedCursorPreview::OnUpdate: Creating empty accumulated cursor preview."))
    
    this->AccumulatedAmount->SetText(FText());
    this->AccumulatedPreview->SetBrushFromTexture(nullptr);
    this->AccumulatedPreview->SetColorAndOpacity(FLinearColor::Transparent);

    return;
}

#undef UIL_LOG
#undef GI
#undef CH
