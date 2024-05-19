// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/GameModeBase.h"
#include "FrontEndGameMode.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AFrontEndGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:

    explicit AFrontEndGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
