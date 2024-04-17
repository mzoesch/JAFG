// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "GenPrevAssetsGameModeBase.generated.h"

UCLASS(NotBlueprintable)
class JAFG_API AGenPrevAssetsGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:

    explicit AGenPrevAssetsGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
