// Copyright 2024 mzoesch. All rights reserved.

#include "System/LocalPlayerChunkGeneratorSubsystem.h"

#include "Network/HyperlaneWorker.h"
#include "Network/NetworkStatics.h"
#include "World/Chunk/CommonChunk.h"

void ULocalPlayerChunkGeneratorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Warning, TEXT("ULocalPlayerChunkGenerator::Initialize: Initialized subsystem."))

    return;
}

void ULocalPlayerChunkGeneratorSubsystem::Deinitialize(void)
{
    Super::Deinitialize();

    this->DisconnectFromHyperlane();

    UE_LOG(LogTemp, Warning, TEXT("ULocalPlayerChunkGenerator::Deinitialize: Deinitialized subsystem."))

    return;
}

void ULocalPlayerChunkGeneratorSubsystem::ConnectWithHyperlane(void)
{
    if (UNetworkStatics::IsSafeClient(this) == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalPlayerChunkGenerator::ConnectWithHyperlane: Not a client."))
        return;
    }

    UE_LOG(LogTemp, Error, TEXT("ULocalPlayerChunkGenerator::ConnectWithHyperlane: Connecting to hyperlane."))
    this->Worker = new FHyperlaneWorker(this);

    return;
}

void ULocalPlayerChunkGeneratorSubsystem::DisconnectFromHyperlane()
{
    if (this->Worker == nullptr)
    {
        return;
    }

    if (UNetworkStatics::IsSafeClient(this) == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalPlayerChunkGeneratorSubsystem::~DisconnectFromHyperlane: Hyperlane found on a non client instance. Something fishy is going on."))
    }

    delete this->Worker;

    return;
}

void ULocalPlayerChunkGeneratorSubsystem::InitializeChunkWithAuthorityData(const FIntVector& InChunkKey, const TArray<int32>& InRawVoxels)
{
    ACommonChunk* Chunk = this->LoadedChunks[InChunkKey];
    if (Chunk == nullptr)
    {
        /* Necessary to also crash the game in shipping builds. */
        UE_LOG(LogTemp, Fatal, TEXT("ULocalPlayerChunkGeneratorSubsystem::InitializeChunkWithAuthorityData: Chunk not found."))
        return;
    }

    Chunk->InitializeWithAuthorityData(InRawVoxels);

    return;
}
