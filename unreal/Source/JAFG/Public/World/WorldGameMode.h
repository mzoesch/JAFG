// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/GameModeBase.h"

#include "WorldGameMode.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AWorldGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:

    explicit AWorldGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
