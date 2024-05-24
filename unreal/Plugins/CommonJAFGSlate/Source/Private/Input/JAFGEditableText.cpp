// Copyright 2024 mzoesch. All rights reserved.

#include "Input/JAFGEditableText.h"

void UJAFGEditableText::SetCustomEventToKeyDown(const FOnKeyDown& InOnKeyDownHandler) const
{
    this->MyEditableText->SetOnKeyDownHandler(InOnKeyDownHandler);
}
