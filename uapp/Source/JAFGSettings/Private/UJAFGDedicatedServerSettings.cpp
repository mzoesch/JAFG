// Copyright 2024 mzoesch. All rights reserved.

#include "UJAFGDedicatedServerSettings.h"

UJAFGDedicatedServerSettings::UJAFGDedicatedServerSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->CategoryName = FName(TEXT("JAFG"));
    this->SectionName  = FName(TEXT("DedicatedServer"));

    return;
}
