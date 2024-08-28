// Copyright 2024 mzoesch. All rights reserved.

#include "Input/JAFGEditableText.h"
#include "DefaultColorsSubsystem.h"
#include "Commands/ChatCommandStatics.h"

void UJAFGEditableText::SetCustomEventToKeyDown(const FOnKeyDown& InOnKeyDownHandler) const
{
    this->MyEditableText->SetOnKeyDownHandler(InOnKeyDownHandler);
}

void UJAFGEditableText::SetMaxSize(const int32 InMaxSize)
{
    if (InMaxSize < 0)
    {
        this->TrimTo = -1;
        jcheck( this->TrimToB < 0 )

        this->OnTextChanged.RemoveDynamic(this, &UJAFGEditableText::OnNativeTextChanged);
    }
    else
    {
        this->TrimTo = InMaxSize;

        if (this->OnTextChanged.IsAlreadyBound(this, &UJAFGEditableText::OnNativeTextChanged) == false)
        {
            this->OnTextChanged.AddDynamic(this, &UJAFGEditableText::OnNativeTextChanged);
        }
    }

    return;
}

void UJAFGEditableText::SetMaxSizeB(const int32 InMaxSize, const TFunction<bool(const FText& InText)>* InTrimFunctionB /* = nullptr */)
{
    if (InMaxSize < 0)
    {
        this->TrimToB = -1;
    }
    else
    {
        jcheck( this->TrimTo >= 0 )

        this->TrimToB       = InMaxSize;
        this->TrimFunctionB = *InTrimFunctionB;

        /*
         * We do not need to dynamically bound to the text changed delegate as the UJAFGEditableText#TrimTo has to
         * be set, and they would have bound to it already.
         */
    }

    return;
}

void UJAFGEditableText::UpdateComponentWithTheirScheme(void)
{
    if (this->ColorScheme == EJAFGFontSize::DontCare && this->FontColorScheme == EJAFGFont::DontCare)
    {
        return;
    }

    const UDefaultColorsSubsystem* Subsystem = this->GetGameInstance()->GetSubsystem<UDefaultColorsSubsystem>();

    FSlateFontInfo Temp   = this->GetFont();
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

void UJAFGEditableText::OnNativeTextChanged(const FText& InText)
{
    if (this->TrimTo < 0 && this->TrimToB < 0)
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
        if (this->TrimToB < 0)
        {
            this->SetText(FText::FromString(InText.ToString().Left(this->TrimTo)));
        }
        else if (this->TrimFunctionB && this->TrimFunctionB(InText))
        {
            if (InText.ToString().Len() > this->TrimToB)
            {
                this->SetText(FText::FromString(InText.ToString().Left(this->TrimToB)));
            }
        }
        else
        {
            this->SetText(FText::FromString(InText.ToString().Left(this->TrimTo)));
        }
    }

    if (this->LastValidText.EqualTo(this->GetText()))
    {
        return;
    }

    this->LastValidText = this->GetText();
    this->OnTrimmedTextChanged.Broadcast(this->GetText());

    return;
}
