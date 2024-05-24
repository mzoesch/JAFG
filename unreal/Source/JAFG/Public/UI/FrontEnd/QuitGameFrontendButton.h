// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "TabBar/JAFGTabBarButtonPopUp.h"

#include "QuitGameFrontendButton.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UQuitGameFrontendButton : public UJAFGTabBarButtonPopUp
{
    GENERATED_BODY()

public:

    explicit UQuitGameFrontendButton(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void OnPopUpClosed(const bool bAccepted) override;
};
