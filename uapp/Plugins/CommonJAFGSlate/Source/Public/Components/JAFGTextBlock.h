// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "UsesJAFGColorScheme.h"
#include "JAFGSlateStatics.h"

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

    FORCEINLINE auto SetColorScheme(const EJAFGFontSize::Type InColorScheme) -> void { this->ColorScheme = InColorScheme; }

protected:

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGFontSize::Type> ColorScheme = EJAFGFontSize::DontCare;
};
