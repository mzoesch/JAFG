// Copyright 2024 mzoesch. All rights reserved.

#include "CommonJAFGSlateDeveloperSettings.h"

UCommonJAFGSlateDeveloperSettings::UCommonJAFGSlateDeveloperSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->CategoryName = FName(TEXT("JAFG"));
    this->SectionName  = FName(TEXT("Slate"));

    return;
}
