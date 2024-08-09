// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGModSettings.h"

UJAFGModSettings::UJAFGModSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->CategoryName = FName(TEXT("JAFGMod"));
    this->SectionName  = FName(TEXT("JAFGMod"));

    return;
}
