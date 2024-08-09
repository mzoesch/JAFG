// Copyright 2024 mzoesch. All rights reserved.

#include "PopUps/LoadingScreen.h"

#include "CommonHUD.h"
#include "RegisteredWorldNames.h"

ULoadingScreen::ULoadingScreen(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void ULoadingScreen::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (this->GetWorld() && this->GetWorld()->GetName() == RegisteredWorlds::Dev)
    {
        this->GetOwningPlayer()->GetHUD<ACommonHUD>()->DestroyLoadingScreen();
    }

    return;
}
