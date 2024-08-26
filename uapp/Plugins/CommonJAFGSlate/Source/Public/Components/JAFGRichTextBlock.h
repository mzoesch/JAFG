// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UsesJAFGColorScheme.h"
#include "Components/RichTextBlock.h"
#include "JAFGSlateStatics.h"

#include "JAFGRichTextBlock.generated.h"

UCLASS(Blueprintable)
class COMMONJAFGSLATE_API UJAFGRichTextBlock : public URichTextBlock, public IUsesJAFGColorScheme
{
    GENERATED_BODY()

public:

    explicit UJAFGRichTextBlock(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // IUserJAFGColorScheme interface
    virtual void UpdateComponentWithTheirScheme(void) override;
    // ~IUserJAFGColorScheme interface

    UFUNCTION(BlueprintCallable)
    void SetColorScheme(const EJAFGFontSize::Type InColorScheme);

    FORCEINLINE auto SetNativeColorScheme(const EJAFGFontSize::Type InColorScheme) -> void { this->ColorScheme     = InColorScheme; }
    FORCEINLINE auto SetNativeColorScheme(const EJAFGRichFont::Type InColorScheme) -> void { this->FontColorScheme = InColorScheme; }

protected:

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGFontSize::Type> ColorScheme = EJAFGFontSize::DontCare;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGRichFont::Type> FontColorScheme = EJAFGRichFont::DontCare;
};
