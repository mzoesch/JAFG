// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGWidget.h"

UJAFGWidget::UJAFGWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->MyCommonForegroundColor             = UJAFGWidget::CommonForegroundColor;
    this->MyCommonForegroundColorReducedAlpha = UJAFGWidget::CommonForegroundColorReducedAlpha;
    this->MyCommonBackgroundColor             = UJAFGWidget::CommonBackgroundColor;

    return;
}
