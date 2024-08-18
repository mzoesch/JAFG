// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Frontend/SettingsTabBar.h"

#include "JAFGSettingsTabBar.generated.h"

class UGameSettingRegistry;

UCLASS(Abstract, Blueprintable)
class JAFGSETTINGS_API UJAFGSettingsTabBar : public USettingsTabBar
{
    GENERATED_BODY()

public:

    explicit UJAFGSettingsTabBar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    // UUserWidget implementation
    virtual void NativeConstruct(void) override;
    // ~UUserWidget implementation
};
