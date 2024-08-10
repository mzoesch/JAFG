// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "JAFGModSettings.generated.h"

UCLASS(Config=JAFGMod, DefaultConfig, meta=(DisplayName="JAFG Mod Settings"))
class JAFGMOD_API UJAFGModSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    explicit UJAFGModSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Slate")
    int PlaceHolder = 0;
};
