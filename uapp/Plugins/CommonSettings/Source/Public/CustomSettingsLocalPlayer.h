// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"

#include "CustomSettingsLocalPlayer.generated.h"

class UJAFGSettingsLocal;

/**
 * Abstract super base class for local players that provide custom settings. Easy to extend to provide user
 * feedback through widgets.
 */
UCLASS(Abstract, NotBlueprintable)
class COMMONSETTINGS_API UCustomSettingsLocalPlayer : public ULocalPlayer
{
    GENERATED_BODY()

public:

    /*
     * Please note.
     * Always use UFUNCTION() to expose a method to the cached dynamic property paths if it should be accessible
     * through the user interface.
     */

    UFUNCTION()
    UJAFGSettingsLocal* GetLocalSettings( /* void */ ) const;
};
