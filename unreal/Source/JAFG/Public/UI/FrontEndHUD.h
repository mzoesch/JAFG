// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "CommonHUD.h"

#include "FrontEndHUD.generated.h"

JAFG_VOID

class UJAFGWidget;

UCLASS(NotBlueprintable)
class JAFG_API AFrontEndHUD : public ACommonHUD
{
    GENERATED_BODY()

public:

    explicit AFrontEndHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

private:

    TObjectPtr<UJAFGWidget> FrontEnd = nullptr;
};
