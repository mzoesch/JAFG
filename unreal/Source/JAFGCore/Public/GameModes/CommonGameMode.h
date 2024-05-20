// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "CommonGameMode.generated.h"

UCLASS(Abstract, NotBlueprintable)
class JAFGCORE_API ACommonGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:

    explicit ACommonGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    bool bStartWithCursorVisible = false;

    //~=============================================================================
    // Player joining and leaving
    virtual auto PostLogin(APlayerController* NewPlayer) -> void override;
};
