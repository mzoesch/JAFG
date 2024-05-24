// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGFocusableUserWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

UJAFGFocusableUserWidget::UJAFGFocusableUserWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGFocusableUserWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->ButtonStyleUnfocused = this->B_Master->GetStyle();

    this->ButtonStyleFocused = FButtonStyle(this->ButtonStyleUnfocused);
    this->ButtonStyleFocused.SetNormal(this->ButtonStyleFocused.Pressed);
    this->ButtonStyleFocused.SetHovered(this->ButtonStyleFocused.Pressed);

    this->B_Master->OnClicked.AddDynamic(this, &UJAFGFocusableUserWidget::OnClickedDynamic);

    return;
}

void UJAFGFocusableUserWidget::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FPassedFocusableWidgetData)
    {
        this->WidgetIdentifier        = Data->WidgetIdentifier;
        this->OnWidgetPressedDelegate = Data->OnWidgetPressedDelegate;
    }

    this->OnWidgetPressedDelegate.CheckCallable();

    return;
}

void UJAFGFocusableUserWidget::SetWidgetFocusWithBool(const bool bFocus)
{
    if (bFocus)
    {
        this->SetWidgetFocus();
    }
    else
    {
        this->SetWidgetUnfocus();
    }
}

void UJAFGFocusableUserWidget::SetWidgetFocus(void)
{
    this->B_Master->SetStyle(this->ButtonStyleFocused);
}

void UJAFGFocusableUserWidget::SetWidgetUnfocus(void)
{
    this->B_Master->SetStyle(this->ButtonStyleUnfocused);
}

void UJAFGFocusableUserWidget::SetTextIfBound(const FString& InText) const
{
    if (this->TB_Master)
    {
        this->TB_Master->SetText(FText::FromString(InText));
    }
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UJAFGFocusableUserWidget::OnClickedDynamic(void)
{
    this->OnWidgetPressedDelegate.CheckCallable();
    this->OnWidgetPressedDelegate(this->WidgetIdentifier);

    return;
}
