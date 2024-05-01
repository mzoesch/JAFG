// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Player/CommonPlayerController.h"

#include "WorldPlayerController.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AWorldPlayerController : public ACommonPlayerController
{
    GENERATED_BODY()

public:

    explicit AWorldPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
