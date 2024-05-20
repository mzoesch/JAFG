// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "CommonPlayerController.generated.h"

UCLASS(NotBlueprintable)
class JAFGCORE_API ACommonPlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    explicit ACommonPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void ShowMouseCursor(const bool bShow, const bool bCenter = true);

    UFUNCTION(Client, Reliable)
    void ShowMouseCursor_ClientRPC(const bool bShow, const bool bCenter = true);
};
