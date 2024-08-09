// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "CurrentWorldInformationSubsystem.generated.h"

JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API UCurrentWorldInformationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    UCurrentWorldInformationSubsystem();

    /** @return True if this client should be able to travel. */
    bool TravelClientAroundWhileNotInAnySession(ERegisteredWorlds::Type InNewWorld) const;
};
