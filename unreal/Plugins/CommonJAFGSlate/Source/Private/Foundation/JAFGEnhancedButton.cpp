// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/JAFGEnhancedButton.h"

#include "Components/JAFGTextBlock.h"

void UJAFGEnhancedButton::SetContent(const FText& InContent)
{
    this->Text_Content->SetText(InContent);
}

FText UJAFGEnhancedButton::GetContent(void) const
{
    return this->Text_Content->GetText();
}
