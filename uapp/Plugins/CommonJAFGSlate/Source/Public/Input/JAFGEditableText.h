// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGSlateStatics.h"
#include "UsesJAFGColorScheme.h"
#include "Components/EditableText.h"
#include "System/FontSubsystem.h"

#include "JAFGEditableText.generated.h"

/**
 * An editable text with additional features.
 * Never bind to the OnTextChanged event if the text should be trimmed.
 * Instead, bind to the OnTrimmedTextChanged event.
 */
UCLASS(Blueprintable)
class COMMONJAFGSLATE_API UJAFGEditableText : public UEditableText, public IUsesJAFGColorScheme
{
    GENERATED_BODY()

public:

    void SetCustomEventToKeyDown(const FOnKeyDown& InOnKeyDownHandler) const;

    DECLARE_EVENT_OneParam(UJAFGEditableText, OnTrimmedTextChangedSignature, const FText& /* InText */)
    OnTrimmedTextChangedSignature OnTrimmedTextChanged;
    void ResetLastValidText(void) { this->LastValidText = FText::GetEmpty(); }

    /** @param InMaxSize The max size allowed. -1 for engine maximum. */
    void SetMaxSize(const int32 InMaxSize);
    /**
     * If StdIn exceeds UJAFGEditableText#TrimTo trim the text to InMaxSize if the length of characters from
     * StdIn exceeds it and the InTrimFunctionB returns true.
     *
     * Requires that UJAFGEditableText#TrimTo is set.
     *
     * @param InMaxSize       The max size allowed. -1 for engine maximum.
     * @param InTrimFunctionB Check if StdIn is relevant and meaningful for the current B-Trim.
     */
    void SetMaxSizeB(const int32 InMaxSize, const TFunction<bool(const FText& InText)>* InTrimFunctionB = nullptr);

    // IUserJAFGColorScheme interface
    virtual void UpdateComponentWithTheirScheme(void) override;
    // ~IUserJAFGColorScheme interface

    FORCEINLINE auto SetColorScheme(const EJAFGFontSize::Type InColorScheme) -> void { this->ColorScheme = InColorScheme; }
    FORCEINLINE auto SetColorScheme(const EJAFGFont::Type InColorScheme) -> void { this->FontColorScheme = InColorScheme; }
    FORCEINLINE auto SetColorScheme(const EJAFGFontTypeFace::Type InColorScheme) -> void { this->TypeFaceColorScheme = InColorScheme; }

protected:

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGFontSize::Type> ColorScheme = EJAFGFontSize::DontCare;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGFont::Type> FontColorScheme = EJAFGFont::DontCare;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGFontTypeFace::Type> TypeFaceColorScheme = EJAFGFontTypeFace::Default;

private:

    UFUNCTION()
    void OnNativeTextChanged(const FText& InText);

    int32 TrimTo                                       = -1;
    int32 TrimToB                                      = -1;
    TFunction<bool(const FText& InText)> TrimFunctionB = nullptr;
    FText LastValidText                                = FText::GetEmpty();
};
