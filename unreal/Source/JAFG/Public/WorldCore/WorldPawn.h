// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/Pawn.h"

#include "WorldPawn.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AWorldPawn : public APawn
{
    GENERATED_BODY()

public:

    explicit AWorldPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void Tick(float DeltaTime) override;

private:

    static constexpr float Timeout = 10.0f;
    void TimeoutDestroy(void);
};
