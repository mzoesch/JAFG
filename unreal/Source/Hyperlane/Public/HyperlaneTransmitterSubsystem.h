// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldCore/JAFGWorldSubsystems.h"
#include "TransmittableData.h"

#include "HyperlaneTransmitterSubsystem.generated.h"

class UHyperlaneComponent;
class FHyperlaneTransmitter;

UCLASS()
class HYPERLANE_API UHyperlaneTransmitterSubsystem : public UJAFGWorldSubsystem
{
    GENERATED_BODY()

public:

    UHyperlaneTransmitterSubsystem();
    virtual ~UHyperlaneTransmitterSubsystem(void) override;

    // WorldSubsystem implementation
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    // ~WorldSubsystem implementation

    void SendChunkInitializationData(const UHyperlaneComponent* Target, TransmittableData::FChunkInitializationData& Data) const;

private:

    FHyperlaneTransmitter* HyperlaneTransmitter = nullptr;
};
