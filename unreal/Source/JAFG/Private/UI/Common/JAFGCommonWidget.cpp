// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Common/JAFGCommonWidget.h"

void UJAFGCommonWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->MyCommonWidgetForegroundColor = this->CommonWidgetForegroundColor;
    this->MyCommonWidgetBackgroundColor = this->CommonWidgetBackgroundColor;

    return;
}
