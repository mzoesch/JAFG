// Copyright 2024 mzoesch. All rights reserved.

#include "FrontEnd/JAFGWidget.h"

void UJAFGWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->MyCommonForegroundColor             = UJAFGWidget::CommonForegroundColor;
    this->MyCommonForegroundColorReducedAlpha = UJAFGWidget::CommonForegroundColorReducedAlpha;
    this->MyCommonBackgroundColor             = UJAFGWidget::CommonBackgroundColor;

    return;
}
