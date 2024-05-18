// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MyCharacterMovementComponent.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API UMyCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:

    explicit UMyCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
