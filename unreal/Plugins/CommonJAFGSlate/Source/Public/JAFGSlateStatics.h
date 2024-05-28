// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "JAFGSlateStatics.generated.h"

UENUM(BlueprintType)
namespace EJAFGColorScheme
{

enum Type : uint8
{
    DontCare,
    Primary,
    PrimaryReducedAlpha,
    Secondary,
    AddedSubMenuColor,
};

}
