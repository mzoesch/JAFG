// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"

#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/CommonChunk.h"
#include "WorldCore/Chunk/GreedyChunk.h"

UChunkGenerationSubsystem::UChunkGenerationSubsystem(void) : Super()
{
    return;
}

void UChunkGenerationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<ULocalChunkWorldSettings>();
    Super::Initialize(Collection);

    return;
}

void UChunkGenerationSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    this->LocalChunkWorldSettings = this->GetWorld()->GetSubsystem<ULocalChunkWorldSettings>();
    check( this->LocalChunkWorldSettings )

    return;
}

void UChunkGenerationSubsystem::SpawnChunk(const FChunkKey& ChunkKey)
{
    LOG_VERY_VERBOSE(LogChunkGeneration, "Spawning chunk at %s.", *ChunkKey.ToString())

    const FTransform TargetedChunkTransform = FTransform(
        FRotator::ZeroRotator,
        FVector(
            ChunkKey.X * WorldStatics::ChunkSize * WorldStatics::JToUScale,
            ChunkKey.Y * WorldStatics::ChunkSize * WorldStatics::JToUScale,
            ChunkKey.Z * WorldStatics::ChunkSize * WorldStatics::JToUScale
        ),
        FVector::OneVector
    );

    ACommonChunk* Chunk = this->GetWorld()->SpawnActor<ACommonChunk>(
        this->LocalChunkWorldSettings->LocalChunkType == EChunkType::Greedy
        ? AGreedyChunk::StaticClass() : ACommonChunk::StaticClass(),
        TargetedChunkTransform
    );

    return;
}
