// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "SettingsData/JAFGInputSubsystem.h"

#include "JAFGSettingsLocal.generated.h"

struct FLoadedInputAction;
/** Base class to store all local settings */
UCLASS(NotBlueprintable)
class COMMONSETTINGS_API UJAFGSettingsLocal : public UGameUserSettings
{
    GENERATED_BODY()

public:

    explicit UJAFGSettingsLocal(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    static auto Get(void) -> UJAFGSettingsLocal*;

    /*
     * Please note.
     * Always use UFUNCTION() to expose a method to the cached dynamic property paths if it should be accessible
     * through the user interface.
     */

    //////////////////////////////////////////////////////////////////////////
    // Audio

    UFUNCTION()
    float GetMasterVolume( /* void */ ) const;

    UFUNCTION()
    void SetMasterVolume(const float InMasterVolume);

protected:

    UPROPERTY(Config)
    float MasterVolume = 1.0f;

    // ~Audio
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Keybindings

public:

    FORCEINLINE auto SetOwningInputSubsystem(UJAFGInputSubsystem* InOwningInputSubsystem) -> void { this->OwningInputSubsystem = InOwningInputSubsystem; }

    FORCEINLINE auto GetAllLoadedInputActions(void) const -> const TArray<FLoadedInputAction>&;

private:

    UPROPERTY()
    TObjectPtr<UJAFGInputSubsystem> OwningInputSubsystem;

    // ~Keybindings
    //////////////////////////////////////////////////////////////////////////
};
