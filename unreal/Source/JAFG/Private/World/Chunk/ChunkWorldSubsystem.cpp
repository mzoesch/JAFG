// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/ChunkWorldSubsystem.h"

bool UChunkWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    LOG_WARNING(LogTemp, "Called")
    return Super::ShouldCreateSubsystem(Outer);
}

void UChunkWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LOG_WARNING(LogTemp, "Called")
}

void UChunkWorldSubsystem::PostInitialize()
{
    Super::PostInitialize();
    LOG_WARNING(LogTemp, "Called")
}

void UChunkWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    LOG_WARNING(LogTemp, "Called")
}

void UChunkWorldSubsystem::Deinitialize()
{
    Super::Deinitialize();
    LOG_WARNING(LogTemp, "Called")
}
