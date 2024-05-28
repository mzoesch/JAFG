// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "JAFGSlateStatics.generated.h"

UENUM(BlueprintType)
namespace EJAFGColorScheme
{

enum Type : uint8
{
    /** The component will be treated normal and will never be updated by the JAFG color subsystem. */
    DontCare,

    Primary,
    PrimaryAlphaMax,
    PrimaryAlphaMid,
    PrimaryAlphaLess,
    Secondary,
};

}
