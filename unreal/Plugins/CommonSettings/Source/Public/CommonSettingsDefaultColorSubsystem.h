// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefaultColorsSubsystem.h"

#include "CommonSettingsDefaultColorSubsystem.generated.h"

UCLASS(NotBlueprintable)
class COMMONSETTINGS_API UCommonSettingsDefaultColorSubsystem : public UDefaultColorsSubsystem
{
    GENERATED_BODY()

protected:

    virtual void GiveSelfToSettings(void) override;
};
