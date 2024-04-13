// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/LocalPlayerChunkGeneratorSubsystem.h"

#include "Network/HyperlaneWorker.h"
#include "Network/NetworkStatics.h"
#include "World/Chunk/CommonChunk.h"

void ULocalPlayerChunkGeneratorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Verbose, TEXT("ULocalPlayerChunkGenerator::Initialize: Initialized subsystem."))

    return;
}

void ULocalPlayerChunkGeneratorSubsystem::Deinitialize(void)
{
    Super::Deinitialize();

    this->DisconnectFromHyperlane();

    UE_LOG(LogTemp, Verbose, TEXT("ULocalPlayerChunkGenerator::Deinitialize: Deinitialized subsystem."))

    return;
}

void ULocalPlayerChunkGeneratorSubsystem::ConnectWithHyperlane(void)
{
    if (UNetworkStatics::IsSafeClient(this) == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalPlayerChunkGenerator::ConnectWithHyperlane: Not a client."))
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("ULocalPlayerChunkGenerator::ConnectWithHyperlane: Connecting to hyperlane."))
    this->Worker = new FHyperlaneWorker(this);

    return;
}

void ULocalPlayerChunkGeneratorSubsystem::DisconnectFromHyperlane(void)
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

    this->Worker = nullptr;

    return;
}

void ULocalPlayerChunkGeneratorSubsystem::InitializeChunkWithAuthorityData(const FIntVector& InChunkKey, const TArray<int32>& InRawVoxels)
{
    if (this->LoadedChunks.Contains(InChunkKey) == false)
    {
        LOG_FATAL(LogChunkValidation, "Server send chunk data for an unknown chunk: %s.", *InChunkKey.ToString())
        return;
    }

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
