// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameModes/CommonGameMode.h"

#include "GenPrevGameMode.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AGenPrevGameMode : public ACommonGameMode
{
    GENERATED_BODY()

public:

    explicit AGenPrevGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
