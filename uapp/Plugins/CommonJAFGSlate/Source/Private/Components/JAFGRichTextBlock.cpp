// Copyright 2024 mzoesch. All rights reserved.

#include "Components/JAFGRichTextBlock.h"
#include "DefaultColorsSubsystem.h"

UJAFGRichTextBlock::UJAFGRichTextBlock(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void RowIterator(const FName& Key, FRichTextStyleRow& Value)
{
    return;
}

void UJAFGRichTextBlock::UpdateComponentWithTheirScheme(void)
{
    if (this->ColorScheme == EJAFGFontSize::DontCare && this->FontColorScheme == EJAFGRichFont::DontCare)
    {
        return;
    }

    /* Only update font size. */
    if (this->ColorScheme != EJAFGFontSize::DontCare && this->FontColorScheme == EJAFGRichFont::DontCare)
    {
        // TODO This is not tested yet.
        check( false && "If you see this message please test the given code below. This very likely does not work." )

        float Size = this->GetGameInstance()->GetSubsystem<UDefaultColorsSubsystem>()->GetTypeByScheme(this->ColorScheme);
        this->GetTextStyleSet()->ForeachRow<FRichTextStyleRow>(TEXT("RichTextStyleTable"), [Size] (const FName& Key, const FRichTextStyleRow& Value) -> void
        {
            const_cast<FRichTextStyleRow&>(Value).TextStyle.Font.Size = Size;
        });

        // How to we update?

        return;
    }

    UDataTable* Table = this->GetGameInstance()->GetSubsystem<UDefaultColorsSubsystem>()->GetTypeByScheme(this->FontColorScheme);

    if (this->ColorScheme == EJAFGFontSize::DontCare)
    {
        this->SetTextStyleSet(Table);
        return;
    }

    float Size = this->GetGameInstance()->GetSubsystem<UDefaultColorsSubsystem>()->GetTypeByScheme(this->ColorScheme);
    Table->ForeachRow<FRichTextStyleRow>(TEXT("RichTextStyleTable"), [Size] (const FName& Key, const FRichTextStyleRow& Value) -> void
    {
        const_cast<FRichTextStyleRow&>(Value).TextStyle.Font.Size = Size;
    });

    this->SetTextStyleSet(Table);

    return;
}

void UJAFGRichTextBlock::SetColorScheme(const EJAFGFontSize::Type InColorScheme)
{
    this->SetNativeColorScheme(InColorScheme);
}
