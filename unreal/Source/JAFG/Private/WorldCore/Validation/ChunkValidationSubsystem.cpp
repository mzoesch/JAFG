// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystem.h"

#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"

UChunkValidationSubsystem::UChunkValidationSubsystem() : Super()
{
    return;
}

void UChunkValidationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<ULocalChunkWorldSettings>();
    Collection.InitializeDependency<UChunkGenerationSubsystem>();
    Super::Initialize(Collection);

    return;
}

void UChunkValidationSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    this->CreateMockChunks();
}

void UChunkValidationSubsystem::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void UChunkValidationSubsystem::CreateMockChunks(void)
{
    UChunkGenerationSubsystem* ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();

    ChunkGenerationSubsystem->SpawnChunk(FChunkKey(0, 0, 0));

    return;
}
