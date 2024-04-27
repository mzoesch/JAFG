// Copyright 2024 mzoesch. All rights reserved.

#include "MenuGameMode.h"

#include "MenuHUD.h"

AMenuGameMode::AMenuGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->HUDClass = AMenuHUD::StaticClass();
}
