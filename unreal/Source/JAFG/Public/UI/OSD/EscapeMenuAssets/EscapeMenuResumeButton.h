// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "TabBar/JAFGTabBarButton.h"

#include "EscapeMenuResumeButton.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API UEscapeMenuResumeButton : public UJAFGTabBarButton
{
    GENERATED_BODY()

public:

    explicit UEscapeMenuResumeButton(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void NativeOnThisTabPressed(void) override;
};
