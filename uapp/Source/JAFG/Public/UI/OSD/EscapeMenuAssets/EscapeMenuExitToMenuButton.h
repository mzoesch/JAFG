// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "TabBar/JAFGTabBarButtonPopUp.h"

#include "EscapeMenuExitToMenuButton.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UEscapeMenuExitToMenuButton : public UJAFGTabBarButtonPopUp
{
    GENERATED_BODY()

public:

    explicit UEscapeMenuExitToMenuButton(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void OnPopUpClosed(const bool bAccepted) override;
};
