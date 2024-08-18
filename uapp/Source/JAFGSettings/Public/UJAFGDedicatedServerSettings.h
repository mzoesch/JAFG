// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UJAFGDedicatedServerSettings.generated.h"

UCLASS(Config=JAFG, DefaultConfig, meta=(DisplayName="JAFG Dedicated Server Settings"))
class JAFGSETTINGS_API UJAFGDedicatedServerSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    explicit UJAFGDedicatedServerSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(Config, EditAnywhere,  BlueprintReadOnly, Category = "Server")
    int32 RenderDistance = 5;
};
