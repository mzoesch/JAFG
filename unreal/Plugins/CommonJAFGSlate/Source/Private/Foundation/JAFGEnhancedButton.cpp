// Copyright 2024 mzoesch. All rights reserved.

#include "Foundation/JAFGEnhancedButton.h"

#include "Components/JAFGButton.h"
#include "Components/JAFGTextBlock.h"

void UJAFGEnhancedButton::NativeConstruct(void)
{
    Super::NativeConstruct();

    this->Button_Root->OnClicked.AddDynamic(this, &UJAFGEnhancedButton::OnNativeButtonClicked);

    return;
}

void UJAFGEnhancedButton::SetContent(const FText& InContent) const
{
    if (this->Text_Content)
    {
        this->Text_Content->SetText(InContent);
    }
}

FText UJAFGEnhancedButton::GetContent(void) const
{
    if (this->Text_Content)
    {
        return this->Text_Content->GetText();
    }

    LOG_WARNING(LogCommonSlate, "Tried to access non-existent optional text block in enhanced button.")
    return FText();
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UJAFGEnhancedButton::OnNativeButtonClicked(void)
{
    this->OnClickedEvent.Broadcast();
}
