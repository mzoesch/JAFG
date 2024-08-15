// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/JAFGContainerSlot.h"
#include "CommonJAFGSlateDeveloperSettings.h"
#include "Container.h"
#include "Components/Image.h"
#include "Components/JAFGBorder.h"
#include "Components/JAFGTextBlock.h"
#include "System/TextureSubsystem.h"
#include "Foundation/JAFGContainerSlotTooltip.h"

UJAFGReadOnlyContainerSlot::UJAFGReadOnlyContainerSlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGReadOnlyContainerSlot::NativeConstruct(void)
{
    Super::NativeConstruct();
    this->Border_Foreground->SetBrushColor(FColor::Transparent);
    return;
}

void UJAFGReadOnlyContainerSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

#if !UE_BUILD_SHIPPING
    if (this->SlotData == nullptr)
    {
        jrelaxedCheckNoEntry()
        return;
    }
#endif /* !UE_BUILD_SHIPPING */

    this->Border_Foreground->SetBrushColor(this->HoverColor);

    if (this->SlotData->GetSlotValue() != Accumulated::Null)
    {
        this->SetToolTip(this->CreateToolTip());
    }

    return;
}

void UJAFGReadOnlyContainerSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    this->Border_Foreground->SetBrushColor(FColor::Transparent);
    this->SetToolTip(nullptr);
    return;
}

FReply UJAFGReadOnlyContainerSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return FReply::Handled();
}

FReply UJAFGReadOnlyContainerSlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UJAFGReadOnlyContainerSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    const UObject& UncastedData = *ListItemObject;
    CAST_PASSED_DATA(UJAFGContainerSlotDataBase)
    {
        this->SlotData = Cast<UJAFGContainerSlotDataBase>(ListItemObject);
    }

    this->RenderSlot();

    return;
}

void UJAFGReadOnlyContainerSlot::OnRefresh(void)
{
    this->RenderSlot();
}

void UJAFGReadOnlyContainerSlot::SetSlotData(UJAFGContainerSlotDataBase* InSlotData)
{
    this->SlotData = InSlotData;
    this->RenderSlot();
    return;
}

void UJAFGReadOnlyContainerSlot::RenderSlot(void)
{
#if !UE_BUILD_SHIPPING
    if (this->SlotData == nullptr)
    {
        jrelaxedCheckNoEntry()
        this->SetToolTip(nullptr);
        return;
    }
#endif /* !UE_BUILD_SHIPPING */

    if (this->SlotData->GetSlotValue() != Accumulated::Null)
    {
        this->Text_Amount->SetText(
            this->SlotData->GetSlotAmount() != 1
            ? FText::FromString(FString::FromInt(this->SlotData->GetSlotAmount()))
            : FText()
        );

        this->Image_Preview->SetBrushFromTexture(
            this->GetGameInstance()->GetSubsystem<UTextureSubsystem>()
                ->GetPreviewTexture2D(this->SlotData->GetSlotValue().AccumulatedIndex
            )
        );
        this->Image_Preview->SetColorAndOpacity(FLinearColor::White);

        if (this->GetToolTip() == nullptr)
        {
            this->SetToolTip(this->CreateToolTip());
        }
        else
        {
            Cast<UJAFGContainerSlotTooltip>(this->GetToolTip())->MarkAsDirtySynchronous();
        }

        return;
    }

    this->Text_Amount->SetText(FText());
    this->Image_Preview->SetBrushFromTexture(nullptr);
    this->Image_Preview->SetColorAndOpacity(FLinearColor::Transparent);
    this->SetToolTip(nullptr);

    return;
}

UJAFGContainerSlotTooltip* UJAFGReadOnlyContainerSlot::CreateToolTip(void)
{
    UJAFGContainerSlotTooltip* Tooltip = CreateWidget<UJAFGContainerSlotTooltip>(
        this->GetWorld(),
        GetDefault<UCommonJAFGSlateDeveloperSettings>()->ContainerSlotTooltipWidgetClass
    );
    Tooltip->PassDataToWidget(FSlotTooltipPassData{ this });

    return Tooltip;
}

UJAFGContainerSlot::UJAFGContainerSlot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGContainerSlot::NativeConstruct(void)
{
    Super::NativeConstruct();
}

FReply UJAFGContainerSlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
#if !UE_BUILD_SHIPPING
    if (this->SlotData == nullptr)
    {
        jrelaxedCheckNoEntry()
        return FReply::Unhandled();
    }
#endif /* !UE_BUILD_SHIPPING */

    IContainerOwner* ContainerOwner = Cast<IContainerOwner>(this->GetOwningPlayerPawn());
    if (ContainerOwner == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Container owner is invalid. Cannot proceed to handle mouse up event.")
        return FReply::Unhandled();
    }

    if (this->GetSlotData().Owner->EasyChangeContainerSoftPredict(
        this,
        ContainerOwner,
        this->GetSlotData().Index,
        [] (const int32 InLambdaIndex, IContainer* InLambdaTarget, IContainerOwner* InLambdaOwner) -> bool
        {
            return InLambdaTarget->GetContainer(InLambdaIndex).OnPrimaryClicked(InLambdaOwner);
        },
        ELocalContainerChange::Primary
    ))
    {
        this->MarkAsDirty();
    }

    return FReply::Handled();
}

void UJAFGContainerSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    Super::NativeOnListItemObjectSet(ListItemObject);

    const UObject& UncastedData = *ListItemObject;
    CAST_PASSED_DATA(UJAFGContainerSlotData)
    {
        /*
         * Just to make sure that this child has received the correct derived class.
         */
    }

    return;
}
