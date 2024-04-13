// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "GameFramework/Info.h"

#include "ChunkMulticasterInfo.generated.h"

class ULocalPlayerChunkGeneratorSubsystem;
JAFG_VOID

UCLASS(NotBlueprintable)
class JAFG_API AChunkMulticasterInfo : public AInfo
{
    GENERATED_BODY()

public:

    explicit AChunkMulticasterInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

private:

    TObjectPtr<ULocalPlayerChunkGeneratorSubsystem> LocalPlayerChunkGeneratorSubsystem;

public:

    UFUNCTION(NetMulticast, Reliable)
    void MulticastChunkModification(const FIntVector& ChunkKey, const FIntVector& LocalVoxel, const int32 VoxelValue);
};
