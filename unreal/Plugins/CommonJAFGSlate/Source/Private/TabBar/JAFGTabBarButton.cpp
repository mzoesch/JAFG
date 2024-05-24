// Copyright 2024 mzoesch. All rights reserved.

#include "TabBar/JAFGTabBarButton.h"

#include "JAFGFocusableUserWidget.h"
#include "Components/TextBlock.h"
#include "TabBar/JAFGTabBar.h"
#include "TabBar/JAFGTabBarBase.h"

void UJAFGTabBarButton::PassDataToWidget(const FWidgetPassData& UncastedData)
{
    CAST_PASSED_DATA(FButtonEntryDescriptor)
    {
        this->OwningTabBar  = Data->Owner;
        this->TabIdentifier = Data->Identifier;
    }

    if (this->OwningTabBar == nullptr)
    {
        LOG_FATAL(LogCommonSlate, "Owning Tab Bar is not set. Falty entry: %s.", *this->TabIdentifier)
        return;
    }

    this->InitializeTab();

    return;
}

void UJAFGTabBarButton::NativeOnThisTabPressed(void)
{
    this->GetCheckedOwningTabBar<UJAFGTabBar>()->OnTabPressed(this->TabIdentifier);
}

void UJAFGTabBarButton::NativeOnFocusTab(void) const
{
    if (this->W_TargetFocusableWidget)
    {
        this->W_TargetFocusableWidget->SetWidgetFocus();
    }
}

void UJAFGTabBarButton::NativeOnUnfocusTab(void) const
{
    if (this->W_TargetFocusableWidget)
    {
        this->W_TargetFocusableWidget->SetWidgetUnfocus();
    }
}

void UJAFGTabBarButton::OnTabPressed(const FString& Identifier)
{
    if (this->TabIdentifier == Identifier)
    {
        this->NativeOnFocusTab();
        this->OnFocusTab();
    }
    else
    {
        this->NativeOnUnfocusTab();
        this->OnUnfocusTab();
    }

    return;
}

void UJAFGTabBarButton::InitializeTab(void)
{
    this->GetCheckedOwningTabBar<UJAFGTabBar>()->OnTabPressedEvent.AddUObject(this, &UJAFGTabBarButton::OnTabPressed);

    if (this->W_TargetFocusableWidget)
    {
        FPassedFocusableWidgetData Data;
        Data.WidgetIdentifier        = 0;
        Data.OnWidgetPressedDelegate = [this] (int32 WidgetIdentifier) { this->NativeOnThisTabPressed(); };
        this->W_TargetFocusableWidget->PassDataToWidget(Data);
        this->W_TargetFocusableWidget->SetTextIfBound(this->TabIdentifier);
    }

    if (this->TB_ButtonText)
    {
        this->TB_ButtonText->SetText(FText::FromString(this->TabIdentifier));
    }

    return;
}
