// Copyright 2024 mzoesch. All rights reserved.

#include "Concretes/CommonBarPanelWidget.h"

UCommonBarPanelWidget::UCommonBarPanelWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UCommonBarPanelWidget::OnNativeMadeVisible(void)
{
    this->OnDeferredConstruct();
}

void UCommonBarPanelWidget::OnNativeMadeCollapsed(void)
{
}
