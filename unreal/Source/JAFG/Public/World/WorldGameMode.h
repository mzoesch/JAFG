// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "WorldGameMode.generated.h"

UCLASS(Abstract, Blueprintable)
class JAFG_API AWorldGameMode : public AGameMode
{
    GENERATED_BODY()

public:

    AWorldGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
