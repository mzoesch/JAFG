// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/PlayerController.h"

#include "CommonPlayerController.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API ACommonPlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    explicit ACommonPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
