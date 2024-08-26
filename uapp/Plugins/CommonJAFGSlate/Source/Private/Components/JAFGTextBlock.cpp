// Copyright 2024 mzoesch. All rights reserved.

#include "Components/JAFGTextBlock.h"
#include "DefaultColorsSubsystem.h"

UJAFGTextBlock::UJAFGTextBlock(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGTextBlock::UpdateComponentWithTheirScheme(void)
{
    if (this->ColorScheme == EJAFGFontSize::DontCare && this->FontColorScheme == EJAFGFont::DontCare)
    {
        return;
    }

    const UDefaultColorsSubsystem* Subsystem = this->GetGameInstance()->GetSubsystem<UDefaultColorsSubsystem>();

    FSlateFontInfo Temp = this->GetFont();
    if (this->ColorScheme != EJAFGFont::DontCare)
    {
        Temp.Size = Subsystem->GetTypeByScheme(this->ColorScheme);
    }
    if (this->FontColorScheme != EJAFGFont::DontCare)
    {
        Subsystem->ParseTypeByScheme(this->FontColorScheme, this->TypeFaceColorScheme, Temp);
    }
    this->SetFont(Temp);

    return;
}

void UJAFGTextBlock::SetColorScheme(const EJAFGFontSize::Type InColorScheme)
{
    this->SetNativeColorScheme(InColorScheme);
}
