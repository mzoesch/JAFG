// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/PlayerController.h"

#include "JAFGPlayerController.generated.h"

JAFG_VOID

/** Super base class for all player controllers. */
UCLASS(NotBlueprintable)
class JAFGCORE_API AJAFGPlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    explicit AJAFGPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void ShowMouseCursor(const bool bShow, const bool bCenter = true);
};
