// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SModificationUBTEditorToolShippingWidget final : public SCompoundWidget
{
public:

    SLATE_BEGIN_ARGS(SModificationUBTEditorToolShippingWidget) { }
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
};
