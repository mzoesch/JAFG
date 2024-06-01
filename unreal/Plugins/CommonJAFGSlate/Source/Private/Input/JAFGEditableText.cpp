// Copyright 2024 mzoesch. All rights reserved.

#include "Input/JAFGEditableText.h"

#include "DefaultColorsSubsystem.h"

void UJAFGEditableText::SetCustomEventToKeyDown(const FOnKeyDown& InOnKeyDownHandler) const
{
    this->MyEditableText->SetOnKeyDownHandler(InOnKeyDownHandler);
}

void UJAFGEditableText::SetMaxSize(const int32 InMaxSize)
{
    if (InMaxSize < 0)
    {
        this->TrimTo = -1;
    }
    else
    {
        this->TrimTo = InMaxSize;
    }

    this->OnTextChanged.AddDynamic(this, &UJAFGEditableText::OnNativeTextChanged);

    return;
}

void UJAFGEditableText::UpdateComponentWithTheirScheme(void)
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

void UJAFGEditableText::OnNativeTextChanged(const FText& InText)
{
    if (this->TrimTo < 0)
    {
        this->OnTrimmedTextChanged.Broadcast(InText);
        return;
    }

    if (InText.IsEmpty())
    {
        if (this->LastValidText.IsEmpty())
        {
            return;
        }

        this->LastValidText = FText::GetEmpty();
        this->OnTrimmedTextChanged.Broadcast(InText);
        return;
    }

    if (InText.ToString().Len() > this->TrimTo)
    {
        this->SetText(FText::FromString(InText.ToString().Left(this->TrimTo)));
    }

    if (this->LastValidText.EqualTo(this->GetText()))
    {
        return;
    }

    this->LastValidText = this->GetText();
    this->OnTrimmedTextChanged.Broadcast(this->GetText());

    return;
}
