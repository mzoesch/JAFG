// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/WorldPlayerState.h"

#include "Net/UnrealNetwork.h"

AWorldPlayerState::AWorldPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AWorldPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AWorldPlayerState::SetPlayerName(const FString& S)
{
    if (UNetStatics::IsSafeClient(this))
    {
        LOG_FATAL(LogWorldController, "Disallowed call on non authority")
        return;
    }

    Super::SetPlayerName(S);
}

FString AWorldPlayerState::GetPlayerNameCustom(void) const
{
    return Super::GetPlayerNameCustom();
}
