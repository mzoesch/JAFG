// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGSlateStatics.h"
#include "UsesJAFGColorScheme.h"
#include "Components/BackgroundBlur.h"

#include "JAFGBackgroundBlur.generated.h"

UCLASS(Blueprintable)
class COMMONJAFGSLATE_API UJAFGBackgroundBlur : public UBackgroundBlur, public IUsesJAFGColorScheme
{
    GENERATED_BODY()

public:

    explicit UJAFGBackgroundBlur(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // IUsesJAFGColorScheme interface
    virtual void UpdateComponentWithTheirScheme(void) override;
    // ~IUsesJAFGColorScheme interface

protected:

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGBlurriness::Type> BlurScheme = EJAFGBlurriness::DontCare;
};
