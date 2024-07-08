// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/CurrentWorldInformationSubsystem.h"

#include "LocalSessionSupervisorSubsystem.h"

UCurrentWorldInformationSubsystem::UCurrentWorldInformationSubsystem(void) : Super()
{
    return;
}

bool UCurrentWorldInformationSubsystem::TravelClientAroundWhileNotInAnySession(ERegisteredWorlds::Type InNewWorld) const
{
    ULocalSessionSupervisorSubsystem* LSSSS = this->GetGameInstance()->GetSubsystem<ULocalSessionSupervisorSubsystem>();
    jcheck( LSSSS )

    if (LSSSS->GetCurrentState() != ECurrentLSSSSState::None)
    {
        return false;
    }

    LSSSS->SafeClientTravel(InNewWorld);

    return true;
}
