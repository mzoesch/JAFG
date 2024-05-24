// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"

#include "JAFGLocalPlayer.generated.h"

class UJAFGSettingsLocal;

UCLASS()
class JAFGCORE_API UJAFGLocalPlayer : public ULocalPlayer
{
    GENERATED_BODY()

public:

    UFUNCTION()
    UJAFGSettingsLocal* GetLocalSettings() const;
};
