// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/WorldPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

AWorldPlayerState::AWorldPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void AWorldPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams SharedParams;
    SharedParams.bIsPushBased = true;

    DOREPLIFETIME_WITH_PARAMS_FAST(AWorldPlayerState, PlayerDisplayName, SharedParams);

    return;
}

void AWorldPlayerState::SetPlayerDisplayName(const FString& InPlayerDisplayName)
{
    if (UNetStatics::IsSafeClient(this))
    {
        LOG_FATAL(LogWorldController, "Disallowed call on non authority")
        return;
    }

    this->PlayerDisplayName = InPlayerDisplayName;
    MARK_PROPERTY_DIRTY_FROM_NAME(AWorldPlayerState, PlayerDisplayName, this);

    return;
}
