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
    InGameOSD,
};

}

UENUM(BlueprintType)
namespace EJAFGBlurriness
{

enum Type : uint8
{
    /** The component will be treated normal and will never be updated by the JAFG color subsystem. */
    DontCare,

    Heavy,
    Medium,
    Light,
    None,
};

}

UENUM(BlueprintType)
namespace EJAFGFontSize
{

enum Type : uint8
{
    /** The component will be treated normal and will never be updated by the JAFG color subsystem. */
    DontCare,

    GargantuanHeader,
    Header,
    SubHeader,
    Body,
    Small,
    Tiny,
};

}
