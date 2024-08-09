// Copyright 2024 mzoesch. All rights reserved.

#include "UI/HUD/QuickSessionPreviewWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Player/WorldPlayerController.h"
#include "WorldCore/WorldGameState.h"

void UQuickSessionPreviewWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->QuickSessionPreviewVisibilityChangedHandle =
        this->GetOwningPlayer<AWorldPlayerController>()->SubscribeToQuickSessionPreviewVisibilityChanged(
            ADD_SLATE_VIS_DELG(UQuickSessionPreviewWidget::OnQuickSessionPreviewVisibilityChanged)
        );

    return;
}

void UQuickSessionPreviewWidget::NativeDestruct(void)
{
    Super::NativeDestruct();

    if (this->GetOwningPlayer<AWorldPlayerController>())
    {
        if (this->GetOwningPlayer<AWorldPlayerController>()->UnSubscribeToQuickSessionPreviewVisibilityChanged(
                this->QuickSessionPreviewVisibilityChangedHandle
            ) == false
        )
        {
            LOG_ERROR(LogCommonSlate, "Failed to unsubscribe from Quick Session Preview Visibility Changed event.")
        }
    }

    return;
}

void UQuickSessionPreviewWidget::UpdateWidget(void)
{
    this->HB_SessionPreviewContainer->ClearChildren();

    /*
     * First adding vertically and then horizontally.
     */

    int32 Index = 0;
    for (const AWorldPlayerState* State : this->GetWorld()->GetGameState<AWorldGameState>()->GetWorldPlayerArray())
    {
        constexpr uint8 MaxVertical = 10;

        /* Other players will just not be seen in the menu then. */
        if (constexpr uint8 MaxHorizontal = 3; Index >= MaxHorizontal * MaxVertical)
        {
            break;
        }

        if (Index % MaxVertical == 0)
        {
            /* Spacer between horizontal boxes. */
            if (this->HB_SessionPreviewContainer->GetAllChildren().IsEmpty() == false)
            {
                USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
                Spacer->SetSize(FVector2D(5.0f, 0.0f));
                this->HB_SessionPreviewContainer->AddChild(Spacer);
            }

            UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
            FSlateBrush Brush = FSlateBrush();
            Brush.DrawAs                       = ESlateBrushDrawType::Type::RoundedBox;
            Brush.TintColor                    = FLinearColor(0.02f, 0.02f, 0.02f, 0.5f);
            Brush.OutlineSettings.Width        = 2.0f;
            Brush.OutlineSettings.Color        = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
            Brush.OutlineSettings.CornerRadii  = FVector4(0.0f, 0.0f, 0.0f, 0.0f);
            Brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::Type::FixedRadius;
            Border->SetBrush(Brush);
            Border->SetPadding(FMargin(5.0f));
            this->HB_SessionPreviewContainer->AddChild(Border);

            /* The vertical box that actually contains all the players. */
            Cast<UBorder>(
                this->HB_SessionPreviewContainer->GetChildAt(this->HB_SessionPreviewContainer->GetChildrenCount() - 1)
            )->AddChild(WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass()));

            /* We add a min width. If all player names are very short, it would look a little bit silly without this. */
            USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
            Spacer->SetSize(FVector2D(250.0f, 0.0f));
            Cast<UPanelWidget>(
                    Cast<UPanelWidget>(this->HB_SessionPreviewContainer->GetChildAt(this->HB_SessionPreviewContainer->GetChildrenCount() - 1)
                )->GetChildAt(
                    Cast<UPanelWidget>(this->HB_SessionPreviewContainer->GetChildAt(this->HB_SessionPreviewContainer->GetChildrenCount() - 1)
                )->GetChildrenCount() - 1)
                    )->AddChild(Spacer);
        }

        UHorizontalBox* PlayerBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

        UTextBlock* PlayerName = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        FSlateFontInfo SharedFont = PlayerName->GetFont(); SharedFont.Size = 16;
        PlayerName->SetText(FText::FromString(State->GetPlayerName()));
        PlayerName->SetFont(SharedFont);
        PlayerBox->AddChild(PlayerName);

        USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
        UHorizontalBoxSlot* SpacerSlot = PlayerBox->AddChildToHorizontalBox(Spacer);
        SpacerSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        Spacer->SetSize(FVector2D(10.0f, 0.0f));

        UTextBlock* PlayerPing = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        PlayerPing->SetText(FText::FromString(FString::Printf(TEXT("%d ms"), static_cast<int32>(State->GetPingInMilliseconds()))));
        PlayerPing->SetFont(SharedFont);
        PlayerBox->AddChild(PlayerPing);


        Cast<UPanelWidget>(
                Cast<UPanelWidget>(this->HB_SessionPreviewContainer->GetChildAt(this->HB_SessionPreviewContainer->GetChildrenCount() - 1)
            )->GetChildAt(
                Cast<UPanelWidget>(this->HB_SessionPreviewContainer->GetChildAt(this->HB_SessionPreviewContainer->GetChildrenCount() - 1)
        )->GetChildrenCount() - 1)
             )->AddChild(PlayerBox);

        ++Index;

        continue;
    }

    return;
}

void UQuickSessionPreviewWidget::OnQuickSessionPreviewVisibilityChanged(const bool bVisible)
{
    if (bVisible)
    {
        this->SetVisibility(ESlateVisibility::HitTestInvisible);
        this->UpdateWidget();
    }
    else
    {
        this->SetVisibility(ESlateVisibility::Collapsed);
    }

    return;
}
