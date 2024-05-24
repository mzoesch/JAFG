// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemDedSv.h"

#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#include "WorldCore/Validation/ChunkValidationSubsystemCl.h"
#include "WorldCore/Validation/ChunkValidationSubsystemLitSv.h"
#include "WorldCore/Validation/ChunkValidationSubsystemStandalone.h"

UChunkValidationSubsystemDedSv::UChunkValidationSubsystemDedSv() : Super()
{
    return;
}

void UChunkValidationSubsystemDedSv::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_DISPLAY(LogChunkValidation, "Called.")

    this->SetTickInterval(2.0f);

    return;
}

bool UChunkValidationSubsystemDedSv::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return UNetStatics::IsSafeDedicatedServer(Outer);
}

void UChunkValidationSubsystemDedSv::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemCl>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystems: Cl.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemLitSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystems: LitSv.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemStandalone>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystems: Standalone.")
    }

    this->ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( this->ChunkGenerationSubsystem )

    return;
}

void UChunkValidationSubsystemDedSv::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    this->LoadUnloadTheirChunks();

    return;
}

void UChunkValidationSubsystemDedSv::LoadUnloadTheirChunks(void) const
{
    constexpr int RenderDistance { 3 };

    if (this->ChunkGenerationSubsystem->GetPendingKillVerticalChunkQueue().IsEmpty() == false)
    {
        LOG_ERROR(LogChunkValidation, "Pending kill vertical chunks is not empty.")
    }

    this->ChunkGenerationSubsystem->ClearVerticalChunkQueue();

    TArray<FChunkKey2> PreferredChunks = TArray<FChunkKey2>();

    if (FConstPlayerControllerIterator It = this->GetWorld()->GetPlayerControllerIterator(); It)
    {
        /* Can be zero if the server is running without any clients. */

        for (; It; ++It)
        {
            const FVector PlayerLocation = It->Get()->GetPawnOrSpectator()->GetActorLocation();
            const FChunkKey2 PlayerChunkKey = ChunkStatics::WorldToVerticalChunkKey(PlayerLocation);
            for (FChunkKey2 Key : Validation::GetAllChunksInDistance(PlayerChunkKey, RenderDistance))
            {
                PreferredChunks.AddUnique(Key);
            }
        }
    }

    // Loading
    //////////////////////////////////////////////////////////////////////////
    int32 NewChunksCounter = 0;
    const TArray<FChunkKey2> PersistentChunks = this->ChunkGenerationSubsystem->GetPersistentVerticalChunks();
    for (const FChunkKey2& Preferred : PreferredChunks)
    {
        if (PersistentChunks.Contains(Preferred) == false)
        {
            this->ChunkGenerationSubsystem->GenerateVerticalChunkAsync(Preferred);
            NewChunksCounter++;
        }
    }

    // Unloading
    //////////////////////////////////////////////////////////////////////////
    int32 UnloadedChunksCounter = 0;
    for (const FChunkKey2& ActiveChunk : this->ChunkGenerationSubsystem->GetVerticalChunks())
    {
        if (PreferredChunks.Contains(ActiveChunk) == false)
        {
            this->ChunkGenerationSubsystem->AddVerticalChunkToPendingKillQueue(ActiveChunk);
            UnloadedChunksCounter++;
        }
    }

#if !UE_BUILD_SHIPPING
    if ((NewChunksCounter == 0 && UnloadedChunksCounter == 0) == false)
    {
        LOG_VERY_VERBOSE(LogChunkValidation, "Decided to load %d and unload %d chunks.", NewChunksCounter, UnloadedChunksCounter)
    }
#endif

    return;
}
