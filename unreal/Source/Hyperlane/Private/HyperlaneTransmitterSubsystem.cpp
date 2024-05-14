// Copyright 2024 mzoesch. All rights reserved.

#include "HyperlaneTransmitterSubsystem.h"

#include "CommonNetworkStatics.h"

UHyperlaneTransmitterSubsystem::UHyperlaneTransmitterSubsystem(void) : Super()
{
    return;
}

UHyperlaneTransmitterSubsystem::~UHyperlaneTransmitterSubsystem(void)
{
    if (this->HyperlaneTransmitter)
    {
        delete this->HyperlaneTransmitter;
    }
    else
    {
        LOG_WARNING(LogHyperlane, "Hyperlane Transmitter was invalid. Could not destory in a controlled and safe manner.")
    }

    return;
}

bool UHyperlaneTransmitterSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return UNetStatics::IsSafeDedicatedServer(Outer) || UNetStatics::IsSafeListenServer(Outer);
}

void UHyperlaneTransmitterSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

#if WITH_EDITOR
    /* PIE might not always clean up the subsystems properly. */
    if (this->HyperlaneTransmitter)
    {
        delete this->HyperlaneTransmitter;
    }
#endif /* WITH_EDITOR */

    this->HyperlaneTransmitter = new FHyperlaneTransmitter(this);

    return;
}

void UHyperlaneTransmitterSubsystem::SendChunkInitializationData(const UHyperlaneComponent* Target, TransmittableData::FChunkInitializationData& Data) const
{
    this->HyperlaneTransmitter->SendChunkInitializationData(Target, Data);
}
