// Copyright 2024 mzoesch. All rights reserved.

#include "Components/JAFGTextBlock.h"

#include "DefaultColorsSubsystem.h"

UJAFGTextBlock::UJAFGTextBlock(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGTextBlock::UpdateComponentWithTheirScheme(void)
{
    if (this->ColorScheme == EJAFGFontSize::DontCare)
    {
        return;
    }

    FSlateFontInfo Temp = this->GetFont();
    Temp.Size = this->GetGameInstance()->GetSubsystem<UDefaultColorsSubsystem>()->GetFontSizeByScheme(this->ColorScheme);
    this->SetFont(Temp);

    return;
}

void UJAFGTextBlock::SetColorScheme(const EJAFGFontSize::Type InColorScheme)
{
    this->SetNativeColorScheme(InColorScheme);
}
