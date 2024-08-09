// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "MyCore.h"
#include "GameFramework/Info.h"

#include "ChunkMulticasterInfo.generated.h"

JAFG_VOID

class UChunkGenerationSubsystem;

UCLASS(NotBlueprintable)
class JAFG_API AChunkMulticasterInfo : public AInfo
{
    GENERATED_BODY()

public:

    explicit AChunkMulticasterInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

    /**
     * The chunk generation subsystem that is active on the owing client. Pointer is always null on any type of server.
     */
    UPROPERTY()
    TObjectPtr<UChunkGenerationSubsystem> LocalChunkGenerationSubsystem;

public:

    UFUNCTION(NetMulticast, Reliable)
    void BroadcastChunkModification_NetMulticastRPC(
        const FIntVector /* FChunkKey */ & ChunkKey,
        const FIntVector /* FVoxelKey */ & LocalVoxelKey,
        const uint32     /* voxel_t   */   Voxel
    );
};
