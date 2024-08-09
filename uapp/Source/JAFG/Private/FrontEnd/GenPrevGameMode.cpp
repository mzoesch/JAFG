// Copyright 2024 mzoesch. All rights reserved.

#include "FrontEnd/GenPrevGameMode.h"
#include "CommonHUD.h"

AGenPrevGameMode::AGenPrevGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->HUDClass = ACommonHUD::StaticClass();

    return;
}
