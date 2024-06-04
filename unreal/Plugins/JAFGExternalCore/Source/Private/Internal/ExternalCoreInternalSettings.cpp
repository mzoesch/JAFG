// Copyright 2024 mzoesch. All rights reserved.

#include "Internal/ExternalCoreInternalSettings.h"

UExternalCoreInternalSettings::UExternalCoreInternalSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->CategoryName = FName(TEXT("JAFG"));
    this->SectionName  = FName(TEXT("ExternalCore"));

    return;
}
