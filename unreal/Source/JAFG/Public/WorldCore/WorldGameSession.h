// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/GameSession.h"

#include "WorldGameSession.generated.h"

class AWorldPlayerController;
JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AWorldGameSession : public AGameSession
{
    GENERATED_BODY()

public:

    explicit AWorldGameSession(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual bool KickPlayer(APlayerController* KickedPlayer, const FText& KickReason) override;
    virtual bool KickPlayer(AWorldPlayerController* KickedPlayer, const FText& KickReason);
};
