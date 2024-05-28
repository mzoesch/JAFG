// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGSlateStatics.h"
#include "Components/Border.h"

#include "JAFGBorder.generated.h"

/** A border that works hand in hand with the default colors subsystem. */
UCLASS(Blueprintable)
class COMMONJAFGSLATE_API UJAFGBorder : public UBorder
{
    GENERATED_BODY()

public:

    explicit UJAFGBorder(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void UpdateBrushWithDefaultValues(void);

protected:

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    TEnumAsByte<EJAFGColorScheme::Type> ColorScheme = EJAFGColorScheme::DontCare;
};
