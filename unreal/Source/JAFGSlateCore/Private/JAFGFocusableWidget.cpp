// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGFocusableWidget.h"

#include "Components/Button.h"

void UJAFGFocusableWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    check( this->B_Master )

    this->ButtonStyleUnfocused = this->B_Master->GetStyle();

    this->ButtonStyleFocused = FButtonStyle(this->ButtonStyleUnfocused);
    this->ButtonStyleFocused.SetNormal(this->ButtonStyleFocused.Pressed);
    this->ButtonStyleFocused.SetHovered(this->ButtonStyleFocused.Pressed);

    this->B_Master->OnPressed.AddDynamic(this, &UJAFGFocusableWidget::OnPressedDynamic);

    return;
}

void UJAFGFocusableWidget::PassDataToWidget(const FMyPassedData& MyPassedData)
{
    if (const FPassedFocusableWidgetData* Data = static_cast<const FPassedFocusableWidgetData*>(&MyPassedData); Data == nullptr)
    {
#if WITH_EDITOR
        LOG_ERROR(LogCommonSlate, "MyPassedData is not of type FPassedFocusableWidgetData.")
#else /* WITH_EDITOR */
        LOG_FATAL(LogCommonSlate, "MyPassedData is not of type FPassedFocusableWidgetData.")
#endif /* !WITH_EDITOR */
        return;
    }
    else
    {
        this->WidgetIdentifier        = Data->WidgetIdentifier;
        this->OnWidgetPressedDelegate = Data->OnWidgetPressedDelegate;
    }

    this->OnWidgetPressedDelegate.CheckCallable();

    return;
}

void UJAFGFocusableWidget::SetWidgetFocusWithBool(const bool bFocus)
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

void UJAFGFocusableWidget::SetWidgetFocus(void)
{
    this->B_Master->SetStyle(this->ButtonStyleFocused);
}

void UJAFGFocusableWidget::SetWidgetUnfocus(void)
{
    this->B_Master->SetStyle(this->ButtonStyleUnfocused);
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UJAFGFocusableWidget::OnPressedDynamic(void)
{
    this->OnWidgetPressedDelegate.CheckCallable();

    this->OnWidgetPressedDelegate(this->WidgetIdentifier);

    return;
}
