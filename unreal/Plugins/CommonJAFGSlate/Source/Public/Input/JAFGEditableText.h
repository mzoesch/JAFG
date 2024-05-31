// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableText.h"

#include "JAFGEditableText.generated.h"

/**
 * An editable text with additional features.
 * Never bind to the OnTextChanged event if the text should be trimmed.
 * Instead, bind to the OnTrimmedTextChanged event.
 */
UCLASS(Blueprintable)
class COMMONJAFGSLATE_API UJAFGEditableText : public UEditableText
{
    GENERATED_BODY()

public:

    void SetCustomEventToKeyDown(const FOnKeyDown& InOnKeyDownHandler) const;

    DECLARE_EVENT_OneParam(UJAFGEditableText, OnTrimmedTextChangedSignature, const FText& /* InText */)
    OnTrimmedTextChangedSignature OnTrimmedTextChanged;

    /** @param InMaxSize The max size allowed. -1 for engine maximum. */
    void SetMaxSize(int32 InMaxSize);

private:

    UFUNCTION()
    void OnNativeTextChanged(const FText& InText);

    int32 TrimTo        = -1;
    FText LastValidText = FText::GetEmpty();
};
