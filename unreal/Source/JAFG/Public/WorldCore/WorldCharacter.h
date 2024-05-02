// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "WorldCharacter.generated.h"

struct FInputActionValue;
class UInputComponent;

UCLASS(NotBlueprintable)
class JAFG_API AWorldCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    explicit AWorldCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    void Test(const FInputActionValue& Value);
};
