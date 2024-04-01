// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "JAFGPlayerController.generated.h"

/** Super abstract base class for all player controllers. */
UCLASS(Abstract)
class JAFG_API AJAFGPlayerController : public APlayerController
{
    GENERATED_BODY()
    
public:

    explicit AJAFGPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void ShowMouseCursor(const bool bShow, const bool bCenter = true);
};
