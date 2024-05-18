// Copyright 2024 mzoesch. All rights reserved.

#include "Input/JAFGEditableTextBlock.h"

void UJAFGEditableTextBlock::SetCustomEventToKeyDown(const FOnKeyDown& InOnKeyDownHandler) const
{
    this->MyEditableText->SetOnKeyDownHandler(InOnKeyDownHandler);
}
