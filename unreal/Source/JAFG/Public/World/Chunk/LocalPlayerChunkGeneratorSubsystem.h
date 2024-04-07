// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"

#include "LocalPlayerChunkGeneratorSubsystem.generated.h"

class ACommonChunk;
class FHyperlaneWorker;

UCLASS(NotBlueprintable)
class JAFG_API ULocalPlayerChunkGeneratorSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;

    void ConnectWithHyperlane(void);
    /** Automatically called if deinitialized. */
    void DisconnectFromHyperlane(void);

    void InitializeChunkWithAuthorityData(const FIntVector& InChunkKey, const TArray<int32>& InRawVoxels);

    TQueue<FIntVector> WaitForAuthorityQueue;
    /*
     * The AActors are loaded, not necessarily generated.
     */
    TMap<FIntVector, ACommonChunk*> LoadedChunks;

private:

    FHyperlaneWorker* Worker = nullptr;
};
