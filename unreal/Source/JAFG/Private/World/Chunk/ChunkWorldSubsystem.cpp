// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/ChunkWorldSubsystem.h"

bool UChunkWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    check( Outer )

    const bool bSuperCreate = Super::ShouldCreateSubsystem(Outer);

    LOG_VERBOSE(LogChunkMisc, "Called.")

    if (bSuperCreate == false)
    {
        return false;
    }

    check( Outer->GetWorld() )

    if (Outer->GetWorld()->GetName() == TEXT("L_World"))
    {
        return true;
    }

    LOG_WARNING(LogChunkMisc, "Subsystem will not be created for world %s.", *Outer->GetWorld()->GetName())

    return false;
}

void UChunkWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LOG_VERBOSE(LogChunkMisc, "Called.")
}

void UChunkWorldSubsystem::PostInitialize(void)
{
    Super::PostInitialize();
    LOG_VERBOSE(LogChunkMisc, "Called.")
}

void UChunkWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    LOG_VERBOSE(LogChunkMisc, "Called.")
}

void UChunkWorldSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
    LOG_VERBOSE(LogChunkMisc, "Called.")
}
