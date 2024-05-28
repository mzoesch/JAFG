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
    this->Text_Content->SetText(InContent);
}

FText UJAFGEnhancedButton::GetContent(void) const
{
    return this->Text_Content->GetText();
}

/* Do NOT convert to const method, as this is a Rider IDEA false positive error. */
// ReSharper disable once CppMemberFunctionMayBeConst
void UJAFGEnhancedButton::OnNativeButtonClicked(void)
{
    this->OnClickedEvent.Broadcast();
}
