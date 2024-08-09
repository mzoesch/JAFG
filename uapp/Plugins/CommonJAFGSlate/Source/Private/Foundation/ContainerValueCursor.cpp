// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/ContainerValueCursor.h"

#include "Container.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/JAFGTextBlock.h"
#include "Components/SizeBox.h"
#include "Engine/UserInterfaceSettings.h"
#include "System/TextureSubsystem.h"

UContainerValueCursor::UContainerValueCursor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UContainerValueCursor::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->ContainerOwner = this->GetOwningPlayerPawn();

    if (this->ContainerOwner == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Owning container owner is invalid.")
        return;
    }

    if (this->ContainerOwner->CursorValue == Accumulated::Null)
    {
        LOG_FATAL(LogCommonSlate, "Cursor value is invalid on owning container owner.")
        return;
    }

    this->Text_Amount->SetText(
        this->ContainerOwner->CursorValue.Amount != 1
        ? FText::FromString(FString::FromInt(ContainerOwner->CursorValue.Amount))
        : FText()
    );

    this->Image_Preview->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    this->Image_Preview->SetBrushFromTexture(this->GetGameInstance()->GetSubsystem<UTextureSubsystem>()->GetPreviewTexture2D(ContainerOwner->CursorValue.AccumulatedIndex));

    this->Canvas_WrapperSlot = Cast<UCanvasPanelSlot>(this->SizeBox_Wrapper->Slot);

    if (this->Canvas_WrapperSlot == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Canvas wrapper slot is invalid.")
        return;
    }

    return;
}

void UContainerValueCursor::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (this->ContainerOwner->CursorValue == Accumulated::Null)
    {
        this->RemoveFromParent();
        return;
    }

    FVector2D ViewportSize;
    this->GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

    const float DPIScaling = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));

    FVector2D MousePosition;
    if (!this->GetOwningPlayer()->GetMousePosition(MousePosition.X, MousePosition.Y))
    {
        return;
    }

    this->Canvas_WrapperSlot->SetPosition(MousePosition / DPIScaling - FVector2D(25.0f, 25.0f));

    return;
}
