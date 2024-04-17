// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GenPrevAssetsCharacter.generated.h"

class UGenPrevAssetsComponent;
class UInputComponent;

UCLASS(NotBlueprintable)
class JAFG_API AGenPrevAssetsCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    explicit AGenPrevAssetsCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:

    TObjectPtr<UGenPrevAssetsComponent> GenPrevAssetsComponent;
};
