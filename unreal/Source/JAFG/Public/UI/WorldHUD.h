// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "CommonHUD.h"

#include "WorldHUD.generated.h"

JAFG_VOID

class UEscapeMenu;

UCLASS(NotBlueprintable)
class JAFG_API AWorldHUD : public ACommonHUD
{
    GENERATED_BODY()

public:

    explicit AWorldHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

private:

    TObjectPtr<UEscapeMenu> EscapeMenu;
};
