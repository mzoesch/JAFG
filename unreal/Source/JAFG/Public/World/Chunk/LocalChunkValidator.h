// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CommonCore.h"
#include "Components/ActorComponent.h"

#include "LocalChunkValidator.generated.h"

class AChunkWorldSettings;
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
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

    virtual void TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

    /**
     * Only valid on server.
     */
    TObjectPtr<AChunkWorldSettings> ChunkWorldSettings = nullptr;

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

    /**
     * Client only.
     */
    TQueue<FIntVector> PreValidationChunkQueue;

    bool bExecuteChunkValidationFuture = false;
    TFuture<void> ChunkValidationFuture;
    UFUNCTION(Server, Reliable, WithValidation)
    void AskServerToSpawnChunk_ServerRPC(const FIntVector& ChunkKey);

    /**
     * Only called on an instance of the game where a local player is present.
     * Meaning clients or the host on a listen server.
     */
    void SafeSpawnChunk(const FIntVector& ChunkKey);

    /**
     * Development only.
     */
    void GenerateMockChunks(void);
    void GenerateMockChunksOnClient(void);
    int MockCursor = 1;
    int CurrentMoveIndex = 0;
    FIntVector2 TargetPoint = FIntVector2(0, 0);
    int TimesToMove = 1;
    bool bFinishedMockingChunkGeneration = false;
};
