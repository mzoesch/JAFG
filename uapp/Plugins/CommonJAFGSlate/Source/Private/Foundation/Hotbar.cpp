// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/Hotbar.h"

#include "Container.h"
#include "JAFGMacros.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/JAFGBorder.h"
#include "Components/JAFGTextBlock.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "System/TextureSubsystem.h"

UHotbar::UHotbar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UHotbar::NativeConstruct(void)
{
    Super::NativeConstruct();
    this->MarkAsDirty();
    return;
}

void UHotbar::OnRefresh(void)
{
    this->RenderHotbar();
}

void UHotbar::RenderHotbar(void)
{
    int32 PredictedOldSlot = 0;
    if (this->OverlaySlot_Selector)
    {
        float LeftPadding = this->OverlaySlot_Selector->GetPadding().Left;

        PredictedOldSlot = (LeftPadding - UHotbar::MinLeftSelectorPadding) / UHotbar::SlotSize;
        if (PredictedOldSlot < 0 || PredictedOldSlot > 9)
        {
            LOG_WARNING(LogCommonSlate, "Invalid slot size: %d.", PredictedOldSlot)
            PredictedOldSlot = 0;
        }
    }

    this->RenderHotbar(PredictedOldSlot);

    return;
}

void UHotbar::RenderHotbar(const int32 SlotIndex)
{
    const IContainer* Container = Cast<IContainer>(this->GetOwningPlayerPawn());
    check( Container )

    UTextureSubsystem* TextureSubsystem = this->GetGameInstance()->GetSubsystem<UTextureSubsystem>();
    check( TextureSubsystem )

    if (this->CanvasSlot_Selector == nullptr || this->OverlaySlot_Selector == nullptr || this->HBox_HotbarContainer == nullptr)
    {
        this->Overlay_Hotbar->ClearChildren();
        this->HBox_HotbarContainer = nullptr;
        this->CanvasSlot_Selector  = nullptr;
        this->OverlaySlot_Selector = nullptr;

        this->HBox_HotbarContainer = NewObject<UHorizontalBox>(this);
        UOverlaySlot* HotbarContainerSlot = this->Overlay_Hotbar->AddChildToOverlay(this->HBox_HotbarContainer);
        HotbarContainerSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
        HotbarContainerSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);

        for (int32 i = 0; i < 10; ++i)
        {
            USizeBox* SizeBox = NewObject<USizeBox>(this);
            SizeBox->SetWidthOverride(UHotbar::SlotSize);
            SizeBox->SetHeightOverride(UHotbar::SlotSize);

            UOverlay* Overlay = NewObject<UOverlay>(this);
            SizeBox->AddChild(Overlay);

            UJAFGBorder* Border_Background = NewObject<UJAFGBorder>(this);
            Border_Background->SetColorScheme(EJAFGColorScheme::InGameOSD);
            Border_Background->UpdateComponentWithTheirScheme();
            FSlateBrush Brush_Background = FSlateBrush();
            Brush_Background.SetResourceObject(TextureSubsystem->GetSafeGUITexture2D(NamedTextures::GeneralContainerSlot));
            Brush_Background.SetImageSize(FVector2D(32.0f));
            Brush_Background.DrawAs = ESlateBrushDrawType::Image;
            Border_Background->SetBrush(Brush_Background);
            UOverlaySlot* BackgroundSlot = Overlay->AddChildToOverlay(Border_Background);
            BackgroundSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
            BackgroundSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

            UImage* Image_Preview = NewObject<UImage>(this);
            UTexture2D* PrevTex = TextureSubsystem->GetPreviewTexture2D(Container->GetContainerValue(i).AccumulatedIndex);
            Image_Preview->SetBrushFromTexture(PrevTex);
            if (PrevTex == nullptr) { Image_Preview->SetColorAndOpacity(FColor::Transparent); }
            UOverlaySlot* ImageSlot = Overlay->AddChildToOverlay(Image_Preview);
            ImageSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
            ImageSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
            ImageSlot->SetPadding(FMargin(4.0f));

            UJAFGTextBlock* Text_Amount = NewObject<UJAFGTextBlock>(this);
            Text_Amount->SetNativeColorScheme(EJAFGFontSize::Small);
            Text_Amount->SetText(Container->GetContainerValue(i).Amount < 2
                ? FText()
                : FText::FromString(FString::FromInt(Container->GetContainerValue(i).Amount))
            );
            UOverlaySlot* TextSlot = Overlay->AddChildToOverlay(Text_Amount);
            TextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
            TextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);
            Text_Amount->UpdateComponentWithTheirScheme();

            this->HBox_HotbarContainer->AddChild(SizeBox);
        }

        this->CanvasSlot_Selector = NewObject<USizeBox>(this);
        this->CanvasSlot_Selector->SetWidthOverride(UHotbar::SelectorSize);
        this->CanvasSlot_Selector->SetHeightOverride(UHotbar::SelectorSize);
        UJAFGBorder* Border_Selector = NewObject<UJAFGBorder>(this);
        Border_Selector->SetColorScheme(EJAFGColorScheme::InGameOSD);
        Border_Selector->UpdateComponentWithTheirScheme();
        FSlateBrush Brush_Selector = FSlateBrush();
        Brush_Selector.SetResourceObject(TextureSubsystem->GetSafeGUITexture2D(NamedTextures::HotbarSelector));
        Brush_Selector.SetImageSize(FVector2D(32.0f));
        Brush_Selector.DrawAs = ESlateBrushDrawType::Image;
        Border_Selector->SetBrush(Brush_Selector);
        this->CanvasSlot_Selector->AddChild(Border_Selector);
        this->OverlaySlot_Selector = this->Overlay_Hotbar->AddChildToOverlay(this->CanvasSlot_Selector);
        this->OverlaySlot_Selector->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
        this->OverlaySlot_Selector->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);

        this->MoveSelectorToSlot(SlotIndex);

        return;
    }

    int32 i = -1;
    for (UWidget* Child : this->HBox_HotbarContainer->GetAllChildren())
    {
        ++i;

        if (Cast<USizeBox>(Child) == nullptr || Cast<UOverlay>(Cast<USizeBox>(Child)->GetChildAt(0)) == nullptr)
        {
            LOG_WARNING(LogCommonSlate, "Invalid child found in hotbar container. Regenerating hotbar.")
            this->Overlay_Hotbar->ClearChildren();
            this->CanvasSlot_Selector  = nullptr;
            this->OverlaySlot_Selector = nullptr;
            this->HBox_HotbarContainer = nullptr;
            this->RenderHotbar();

            return;
        }

        for (UWidget* SubChild : Cast<UOverlay>(Cast<USizeBox>(Child)->GetChildAt(0))->GetAllChildren())
        {
            if (UImage* Image_Preview = Cast<UImage>(SubChild); Image_Preview)
            {
                UTexture2D* PrevTex = TextureSubsystem->GetPreviewTexture2D(Container->GetContainerValue(i).AccumulatedIndex);
                Image_Preview->SetBrushFromTexture(PrevTex);
                if (PrevTex == nullptr)
                {
                    Image_Preview->SetColorAndOpacity(FColor::Transparent);
                }
                else
                {
                    Image_Preview->SetColorAndOpacity(FColor::White);
                }

                continue;
            }

            if (UJAFGTextBlock* Text_Amount = Cast<UJAFGTextBlock>(SubChild); Text_Amount)
            {
                Text_Amount->SetText(Container->GetContainerValue(i).Amount < 2
                    ? FText()
                    : FText::FromString(FString::FromInt(Container->GetContainerValue(i).Amount))
                );

                continue;
            }

        }

        continue;
    }

    this->MoveSelectorToSlot(SlotIndex);

    return;
}

void UHotbar::MoveSelectorToSlot(const int32 SlotIndex)
{
    if (SlotIndex < 0 || SlotIndex > 9)
    {
        LOG_WARNING(LogCommonSlate, "Invalid slot index provided: %d.", SlotIndex)
        return;
    }

    if (this->OverlaySlot_Selector == nullptr)
    {
        LOG_WARNING(LogCommonSlate, "Selector slot is invalid. Regenrating hotbar.")
        this->RenderHotbar();
        jcheck( this->OverlaySlot_Selector )
    }

    this->OverlaySlot_Selector->SetPadding(FMargin(UHotbar::MinLeftSelectorPadding + (SlotIndex * UHotbar::SlotSize), 0.0f, 0.0f, -4.0f));

    return;
}
