// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "JAFGSlateStatics.h"
#include "UObject/Interface.h"

#include "UsesJAFGColorScheme.generated.h"

UINTERFACE()
class UUsesJAFGColorScheme : public UInterface
{
    GENERATED_BODY()
};

/**
 * For components only. Not for user widgets.
 * Describes a component that uses the JAFG color scheme.
 */
class COMMONJAFGSLATE_API IUsesJAFGColorScheme
{
    GENERATED_BODY()

public:

    virtual void UpdateBrushWithSetColorScheme(void) PURE_VIRTUAL(IUsesJAFGColorScheme::UpdateBrushWithSetColorScheme)
};
