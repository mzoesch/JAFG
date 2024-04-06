// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldGeneratorInfo.h"

#include "Network/BackgroundChunkUpdaterComponent.h"
#include "Network/NetworkStatics.h"
#include "World/Chunk/GreedyChunk.h"

AWorldGeneratorInfo::AWorldGeneratorInfo(const FObjectInitializer& ObjectInitializer)
{
    this->PrimaryActorTick.bCanEverTick = true;
    this->PrimaryActorTick.bStartWithTickEnabled = true;
    this->PrimaryActorTick.bAllowTickOnDedicatedServer = true;
    this->PrimaryActorTick.TickInterval = 1.0f;

    this->bReplicates = false;
    this->bNetLoadOnClient = true;

    this->FullyLoadedChunks = TMap<FIntVector, ACommonChunk*>();
    this->ChunkGenerationQueue.Empty();

    return;
}

void AWorldGeneratorInfo::BeginPlay(void)
{
    Super::BeginPlay();

    if (UNetworkStatics::IsSafeClient(this))
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldGeneratorInfo::BeginPlay(): Running in client mode."))
        this->bClientMode = true;
    }
    else
    {
        this->bClientMode = false;
    }

    this->FullyLoadedChunks.Empty();
    this->ChunkGenerationQueue.Empty();
    this->PreBackgroundChunkUpdaterComponentInitializationQueue.Empty();

    if (this->bClientMode)
    {
        this->GenerateWorldAsync();
    }

    /*
     * The server will generate the chunks based on the needs of a client.
     */

    return;
}

void AWorldGeneratorInfo::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (this->ChunkGenerationQueue.IsEmpty())
    {
        return;
    }

    for (int i = 0; i < 20; ++i)
    {
        FIntVector Key;
        if (this->ChunkGenerationQueue.Dequeue(Key) == false)
        {
            UE_LOG(LogTemp, Warning, TEXT("AWorldGeneratorInfo::Tick(): Dequeue failed. World generation is complete for this tick."))
            return;
        }

        const FTransform TargetedChunkTransform = FTransform(
            FRotator::ZeroRotator,
            FVector(
                Key.X * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale,
                Key.Y * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale,
                Key.Z * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale
            ),
            FVector::OneVector
        );

        ACommonChunk* Chunk = this->GetWorld()->SpawnActor<ACommonChunk>(AGreedyChunk::StaticClass(), TargetedChunkTransform);

        /* We of course have to first add this to the sweep steps later. */
        this->FullyLoadedChunks.Add(Key, Chunk);

        continue;
    }

    return;
}

void AWorldGeneratorInfo::EnqueueInitializationChunk(FIntVector ChunkKey)
{
    if (this->BackgroundChunkUpdaterComponent == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldGeneratorInfo::EnqueueInitializationChunk: BackgroundChunkUpdaterComponent is nullptr. Adding to pre initialization queue."))
        this->PreBackgroundChunkUpdaterComponentInitializationQueue.Enqueue(ChunkKey);
        return;
    }

    this->BackgroundChunkUpdaterComponent->ChunkInitializationQueue.Enqueue(ChunkKey);

}

void AWorldGeneratorInfo::GenerateChunkForClient(FIntVector ChunkKey, UBackgroundChunkUpdaterComponent* Callback)
{
    check ( Callback )

    const ACommonChunk* Chunk = this->FullyLoadedChunks.Contains(ChunkKey) ? this->FullyLoadedChunks[ChunkKey] : nullptr;

    if (Chunk == nullptr)
    {
        const FTransform TargetedChunkTransform = FTransform(
            FRotator::ZeroRotator,
            FVector(
                ChunkKey.X * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale,
                ChunkKey.Y * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale,
                ChunkKey.Z * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale
            ),
            FVector::OneVector
        );

        Chunk = this->GetWorld()->SpawnActor<ACommonChunk>(AGreedyChunk::StaticClass(), TargetedChunkTransform);
    }

    check( Chunk )

    Chunk->SendInitializationDataToClient(Callback);

    return;
}

void AWorldGeneratorInfo::SetBackgroundChunkUpdaterComponent(UBackgroundChunkUpdaterComponent* InBackgroundChunkUpdaterComponent)
{
    check( InBackgroundChunkUpdaterComponent )

    if (UNetworkStatics::IsSafeClient(this) == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldGeneratorInfo::SetBackgroundChunkUpdaterComponent: Tried to set on a server."))
        return;
    }

    if (InBackgroundChunkUpdaterComponent == nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldGeneratorInfo::SetBackgroundChunkUpdaterComponent: Tried to set a nullptr."))
        return;
    }

    if (this->BackgroundChunkUpdaterComponent != nullptr)
    {
        UE_LOG(LogTemp, Fatal, TEXT("AWorldGeneratorInfo::SetBackgroundChunkUpdaterComponent: Tried to set a non nullptr."))
        return;
    }

    this->BackgroundChunkUpdaterComponent = InBackgroundChunkUpdaterComponent;

    if (this->PreBackgroundChunkUpdaterComponentInitializationQueue.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("AWorldGeneratorInfo::OnBackgroundChunkUpdaterComponentSet: PreBackgroundChunkUpdaterComponentInitializationQueue is empty. Nothing to add."))
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("AWorldGeneratorInfo::OnBackgroundChunkUpdaterComponentSet: Adding pre initialization queue to BackgroundChunkUpdaterComponent."))

    while (this->PreBackgroundChunkUpdaterComponentInitializationQueue.IsEmpty() == false)
    {
        FIntVector Item;
        if (this->PreBackgroundChunkUpdaterComponentInitializationQueue.Dequeue(Item) == false)
        {
            UE_LOG(LogTemp, Error, TEXT("AWorldGeneratorInfo::OnBackgroundChunkUpdaterComponentSet: Dequeue failed."))
            return;
        }

        this->BackgroundChunkUpdaterComponent->ChunkInitializationQueue.Enqueue(Item);

        continue;
    }

    return;
}

void AWorldGeneratorInfo::GenerateWorldAsync()
{
    auto MoveCursorRight = [] (const FIntVector2& CursorLocation)
    {
        return FIntVector2(CursorLocation.X + 1, CursorLocation.Y);
    };

    auto MoveCursorDown = [] (const FIntVector2& CursorLocation)
    {
        return FIntVector2(CursorLocation.X, CursorLocation.Y - 1);
    };

    auto MoveCursorLeft = [] (const FIntVector2& CursorLocation)
    {
        return FIntVector2(CursorLocation.X - 1, CursorLocation.Y);
    };

    auto MoveCursorUp = [] (const FIntVector2& CursorLocation)
    {
        return FIntVector2(CursorLocation.X, CursorLocation.Y + 1);
    };

    const auto Moves = TArray<FIntVector2(*)(const FIntVector2&)>(
    {
        MoveCursorRight, MoveCursorDown, MoveCursorLeft, MoveCursorUp
    });

    int CurrentMoveIndex = 0;

    int n = 1;
    FIntVector2 TargetPoint = FIntVector2(0, 0);
    int TimesToMove = 1;

    for (int Z = this->ChunksAboveZero; Z >= 0; --Z)
    {
        const FIntVector Key = FIntVector(0, 0, Z);
        this->ChunkGenerationQueue.Enqueue(Key);
    }

    while (true)
    {
        for (int _ = 0; _ < 2; ++_)
        {
            CurrentMoveIndex = (CurrentMoveIndex + 1) % Moves.Num();
            for (int __ = 0; __ < TimesToMove; ++__)
            {
                if (n >= this->MaxSpiralPoints)
                {
                    return;
                }

                TargetPoint = Moves[CurrentMoveIndex](TargetPoint);

                ++n;
                for (int Z = this->ChunksAboveZero; Z >= 0; --Z)
                {
                    const FIntVector Key = FIntVector(TargetPoint.X, TargetPoint.Y, Z);
                    this->ChunkGenerationQueue.Enqueue(Key);
                }

                continue;
            }

            continue;
        }

        ++TimesToMove;

        continue;
    }
}
