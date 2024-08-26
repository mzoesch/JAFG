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
    UDataTable* Table = this->GetGameInstance()->GetSubsystem<UDefaultColorsSubsystem>()->GetRichFontByScheme(this->FontColorScheme);

    if (this->ColorScheme == EJAFGFontSize::DontCare)
    {
        this->SetTextStyleSet(Table);
        return;
    }

    float Size = this->GetGameInstance()->GetSubsystem<UDefaultColorsSubsystem>()->GetFontSizeByScheme(this->ColorScheme);
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
