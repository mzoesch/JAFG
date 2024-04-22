// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "GameFramework/Actor.h"

#include "Drop.generated.h"

class ACuboid;
JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API ADrop : public AActor
{
    GENERATED_BODY()

public:

    explicit ADrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> SceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ACuboid> Cuboid;
};
