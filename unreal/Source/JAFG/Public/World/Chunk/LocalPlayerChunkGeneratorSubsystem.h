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

    // ULocalPlayerSubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize(void) override;
    // ~ULocalPlayerSubsystem interface

    void ConnectWithHyperlane(void);
    /** Automatically called if deinitialized. */
    void DisconnectFromHyperlane(void);

    /**
     * Overrides all voxels in a given chunk with the given data. Must only be called in the client.
     */
    void InitializeChunkWithAuthorityData(const FIntVector& InChunkKey, const TArray<int32>& InRawVoxels);

    TQueue<FIntVector> WaitForAuthorityQueue;
    /*
     * The AActors are loaded, not necessarily generated.
     */
    TMap<FIntVector, ACommonChunk*> LoadedChunks;

private:

    FHyperlaneWorker* Worker = nullptr;
};
