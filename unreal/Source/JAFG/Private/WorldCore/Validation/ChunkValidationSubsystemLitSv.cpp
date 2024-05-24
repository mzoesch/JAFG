// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemLitSv.h"

#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#include "WorldCore/Validation/ChunkValidationSubsystemCl.h"
#include "WorldCore/Validation/ChunkValidationSubsystemDedSv.h"
#include "WorldCore/Validation/ChunkValidationSubsystemStandalone.h"
#if WITH_EDITOR
    #include "Editor.h"
#endif /* WITH_EDITOR */

UChunkValidationSubsystemLitSv::UChunkValidationSubsystemLitSv() : Super()
{
    return;
}

void UChunkValidationSubsystemLitSv::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<ULocalChunkWorldSettings>();
    Collection.InitializeDependency<UChunkGenerationSubsystem>();
    Super::Initialize(Collection);

    LOG_DISPLAY(LogChunkValidation, "Called.")

    this->SetTickInterval(2.0f);

    return;
}

bool UChunkValidationSubsystemLitSv::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return UNetStatics::IsSafeListenServer(Outer);
}

void UChunkValidationSubsystemLitSv::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemCl>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystems: Cl.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemDedSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystems: DedSv.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemStandalone>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystems: Standalone.")
    }

    this->ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( this->ChunkGenerationSubsystem )

    return;
}

void UChunkValidationSubsystemLitSv::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    this->LoadUnloadMyAndTheirChunks();

    return;
}

void UChunkValidationSubsystemLitSv::LoadUnloadMyAndTheirChunks(void) const
{
    constexpr int RenderDistance { 10 };

    TArray<FChunkKey2> PreferredChunks = TArray<FChunkKey2>();
    if (FConstPlayerControllerIterator It = this->GetWorld()->GetPlayerControllerIterator(); It)
    {
        for (; It; ++It)
        {
            /* May be null if state of controller is still pending. */
            const APawn* Pawn = It->Get()->GetPawnOrSpectator();
            if (Pawn == nullptr)
            {
                continue;
            }

            for (
                FChunkKey2 Key
                :
                Validation::GetAllChunksInDistance(ChunkStatics::WorldToVerticalChunkKey(Pawn->GetActorLocation()), RenderDistance)
            )
            {
                PreferredChunks.AddUnique(Key);
            }
        }
    }

    /* Copied for faster access. */
    const TArray<FChunkKey2> PersistentChunks = this->ChunkGenerationSubsystem->GetPersistentVerticalChunks();

    // Loading
    //////////////////////////////////////////////////////////////////////////
    int32 NewChunksCounter = 0;
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
    for (const FChunkKey2& ActiveChunk : PersistentChunks)
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
