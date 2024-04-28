// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/GameModeBase.h"

#include "MenuGameMode.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class MENU_API AMenuGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:

    explicit AMenuGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;
};
