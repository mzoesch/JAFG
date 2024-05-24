// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"

#include "JAFGSettingsLocal.generated.h"

UCLASS(NotBlueprintable)
class JAFGSETTINGS_API UJAFGSettingsLocal : public UGameUserSettings
{
    GENERATED_BODY()

public:

    explicit UJAFGSettingsLocal(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    static UJAFGSettingsLocal* Get();

    float GetMasterVolume(void) const;
    void SetMasterVolume(float InMasterVolume);

protected:

    UPROPERTY(Config)
    float MasterVolume = 1.0f;
};
