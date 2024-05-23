// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "GameSettingRegistry.generated.h"

UCLASS(Abstract, NotBlueprintable)
class COMMONSETTINGS_API UGameSettingRegistry : public UObject
{
    GENERATED_BODY()

public:

    explicit UGameSettingRegistry(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void Initialize(ULocalPlayer* InLocalPlayer);

    UPROPERTY()
    TObjectPtr<ULocalPlayer> OwingLocalPlayer;
};
