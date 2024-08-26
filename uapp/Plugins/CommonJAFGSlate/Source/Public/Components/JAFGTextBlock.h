// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "UsesJAFGColorScheme.h"
#include "JAFGSlateStatics.h"
#include "System/FontSubsystem.h"

#include "JAFGTextBlock.generated.h"

UCLASS(Blueprintable)
class COMMONJAFGSLATE_API UJAFGTextBlock : public UTextBlock, public IUsesJAFGColorScheme
{
    GENERATED_BODY()

public:

    explicit UJAFGTextBlock(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // IUserJAFGColorScheme interface
    virtual void UpdateComponentWithTheirScheme(void) override;
    // ~IUserJAFGColorScheme interface

    UFUNCTION(BlueprintCallable)
    void SetColorScheme(const EJAFGFontSize::Type InColorScheme);

    FORCEINLINE auto SetNativeColorScheme(const EJAFGFontSize::Type InColorScheme) -> void { this->ColorScheme     = InColorScheme;     }
    FORCEINLINE auto SetNativeColorScheme(const EJAFGFont::Type InFontColorScheme) -> void { this->FontColorScheme = InFontColorScheme; }
    FORCEINLINE auto SetNativeColorScheme(const EJAFGFontTypeFace::Type InTypeFaceColorScheme) -> void { this->TypeFaceColorScheme = InTypeFaceColorScheme; }

protected:

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGFontSize::Type> ColorScheme = EJAFGFontSize::DontCare;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGFont::Type> FontColorScheme = EJAFGFont::DontCare;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGFontTypeFace::Type> TypeFaceColorScheme = EJAFGFontTypeFace::Default;
};
