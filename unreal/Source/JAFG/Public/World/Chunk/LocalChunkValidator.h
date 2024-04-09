// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Components/ActorComponent.h"

#include "LocalChunkValidator.generated.h"

class AWorldGeneratorInfo;
class ULocalPlayerChunkGeneratorSubsystem;
JAFG_VOID

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JAFG_API ULocalChunkValidator : public UActorComponent
{
    GENERATED_BODY()

public:

    explicit ULocalChunkValidator(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    virtual void BeginPlay(void) override;

public:

    virtual void TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

    /**
     * Valid on client and listen server for the corresponding local player.
     */
    TObjectPtr<ULocalPlayerChunkGeneratorSubsystem> ChunkGeneratorSubsystem = nullptr;
    /**
     * Only valid on the server.
     */
    TObjectPtr<AWorldGeneratorInfo> WorldGeneratorInfo = nullptr;

    /**
     * Server only.
     *
     * All chunk generation finished delegate callbacks for this client are stored here.
     */
    TMap<FIntVector, FDelegateHandle> ChunkHandles;

    UFUNCTION(Server, Reliable, WithValidation)
    void AskServerToSpawnChunk_ServerRPC(const FIntVector& ChunkKey);

    /**
     * Only called on a instance of the game where a local player is present.
     * Meaning clients or the host on a listen server.
     */
    void SafeSpawnChunk(const FIntVector& ChunkKey);

    /**
     * Development only.
     */
    void GenerateMockChunks(void);
    int MockChunksAdded = 0;
    bool bFinishedMockingChunkGeneration = false;
};
