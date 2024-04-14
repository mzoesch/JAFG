// Copyright 2024 mzoesch. All rights reserved.

#include "UI/Common/JAFGCommonDirtyWidget.h"

void UJAFGCommonDirtyWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->bDirty = false;

    return;
}

void UJAFGCommonDirtyWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (this->bDirty)
    {
        this->bDirty = false;
        this->OnRefresh();
    }

    return;
}
