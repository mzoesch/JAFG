// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"

#include "JAFGSettingsLocal.generated.h"

/** Base class to store all local settings */
UCLASS(NotBlueprintable)
class COMMONSETTINGS_API UJAFGSettingsLocal : public UGameUserSettings
{
    GENERATED_BODY()

public:

    explicit UJAFGSettingsLocal(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    static UJAFGSettingsLocal* Get();

    /*
     * Please note.
     * Always use UFUNCTION() to expose a method to the cached dynamic property paths if it should be accessible
     * through the user interface.
     */

    UFUNCTION()
    float GetMasterVolume( /* void */ ) const;
    UFUNCTION()
    void SetMasterVolume(const float InMasterVolume);

protected:

    UPROPERTY(Config)
    float MasterVolume = 1.0f;
};
