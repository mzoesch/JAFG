// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/PlayerState.h"

#include "WorldPlayerState.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AWorldPlayerState : public APlayerState
{
    GENERATED_BODY()

public:

    explicit AWorldPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual auto SetPlayerName(const FString& S) -> void override;
    virtual auto GetPlayerNameCustom(void) const -> FString override;
};
