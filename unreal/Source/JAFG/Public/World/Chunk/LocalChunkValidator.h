// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "LocalChunkValidator.generated.h"

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
     * Server only.
     *
     * All chunk generation finished delegate callbacks for this client are stored here.
     */
    TMap<FIntVector, FDelegateHandle> ChunkHandles;

    UFUNCTION(Server, Reliable)
    void AskServerToSpawnChunk_ServerRPC(const FIntVector& ChunkKey);

    /**
     * Development only.
     */
    void GenerateMockChunks(void);
    int MockChunksAdded = 0;
    bool bFinishedMockingChunkGeneration = false;
};
