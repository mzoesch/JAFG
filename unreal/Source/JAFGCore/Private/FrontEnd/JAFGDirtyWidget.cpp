// Copyright 2024 mzoesch. All rights reserved.

#include "FrontEnd/JAFGDirtyWidget.h"

void UJAFGDirtyWidget::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->bDirty = true;

    return;
}

void UJAFGDirtyWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (this->bDirty)
    {
        this->bDirty = false;
        this->OnRefresh();
    }

    return;
}
