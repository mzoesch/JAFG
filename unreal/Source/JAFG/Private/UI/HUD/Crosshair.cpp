// Copyright 2024 mzoesch. All rights reserved.

#include "UI/HUD/Crosshair.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

UCrosshair::UCrosshair(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UCrosshair::NativeConstruct(void)
{
    Super::NativeConstruct();
    this->MarkAsDirty();
    GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &UCrosshair::OnViewportChangedDelegate);
    return;
}

void UCrosshair::OnRefresh(void)
{
    this->O_CrosshairContainer->ClearChildren();

    UBorder* HorizontalLine = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("HorizontalLine"));
    UBorder* VerticalLine   = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("VerticalLine"));

    HorizontalLine->SetPadding(FMargin(6.0f, 1.0f, 6.0f, 1.0f));
    VerticalLine->SetPadding(FMargin(1.0f, 6.0f, 1.0f, 6.0f));

    UOverlaySlot* HSlot = this->O_CrosshairContainer->AddChildToOverlay(HorizontalLine);
    UOverlaySlot* VSlot = this->O_CrosshairContainer->AddChildToOverlay(VerticalLine);

    HSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
    HSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
    VSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
    VSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);

    const float Scale = UWidgetLayoutLibrary::GetViewportScale(this);

    HorizontalLine->SetDesiredSizeScale(FVector2D(1.0f / Scale, 1.0f / Scale));
    VerticalLine->SetDesiredSizeScale(FVector2D(1.0f / Scale, 1.0f / Scale));

    return;
}

void UCrosshair::OnViewportChangedDelegate(FViewport* Viewport, const uint32 ViewportType)
{
    this->MarkAsDirty();
}
