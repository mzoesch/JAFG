// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "GameFramework/GameMode.h"

#include "WorldGameMode.generated.h"

JAFG_VOID

UCLASS(Abstract, Blueprintable)
class JAFG_API AWorldGameMode : public AGameMode
{
    GENERATED_BODY()

public:

    explicit AWorldGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void PostLogin(APlayerController* NewPlayer) override;
};
