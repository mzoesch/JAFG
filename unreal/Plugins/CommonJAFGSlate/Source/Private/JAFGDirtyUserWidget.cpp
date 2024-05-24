// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGDirtyUserWidget.h"

void UJAFGDirtyUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    this->bDirty = false;

    return;
}

void UJAFGDirtyUserWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (this->bDirty)
    {
        this->bDirty = false;
        this->OnRefresh();
    }

    return;
}
