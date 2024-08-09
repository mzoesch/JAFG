// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemDedSv.h"

#include "Player/WorldPlayerController.h"
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
    /* Can be zero if the server is running without any clients. */
    if (FConstPlayerControllerIterator It = this->GetWorld()->GetPlayerControllerIterator(); It)
    {
        for (; It; ++It)
        {
            const APlayerController* const UncastedPlayerController = It->Get();
            if (UncastedPlayerController == nullptr)
            {
                continue;
            }
            const AWorldPlayerController* const PlayerController = Cast<AWorldPlayerController>(UncastedPlayerController);
            jcheck( PlayerController )

            FVector PredictedLocation;
            if (PlayerController->GetPredictedCharacterLocation(PredictedLocation) == false)
            {
                continue;
            }

            for (
                FChunkKey2 Key
                :
                Validation::GetAllChunksInDistance(ChunkStatics::WorldToVerticalChunkKey(PredictedLocation), RenderDistance)
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

    /* Can be zero if the server is running without any clients. */
    if (FConstPlayerControllerIterator It = this->GetWorld()->GetPlayerControllerIterator(); It)
    {
        for (; It; ++It)
        {
            APlayerController* UncastedPlayerController = It->Get();
            if (UncastedPlayerController == nullptr)
            {
                continue;
            }
            AWorldPlayerController* PlayerController = Cast<AWorldPlayerController>(UncastedPlayerController);
            jcheck( PlayerController )

            if (PlayerController->HasSuccessfullySpawnedCharacter())
            {
                continue;
            }

            if (PlayerController->IsServerReadyForCharacterSpawn() == false)
            {
                FVector PredictedLocation;
                if (PlayerController->GetPredictedCharacterLocation(PredictedLocation) == false)
                {
                    continue;
                }

                if (this->ChunkGenerationSubsystem->HasPersistentVerticalChunk(FChunkKey2(ChunkStatics::WorldToVerticalChunkKey(PredictedLocation))) == false)
                {
                    return;
                }
                PlayerController->SetServerReadyForCharacterSpawn();
            }

            if (PlayerController->IsClientReadyForCharacterSpawn() == false)
            {
                continue;
            }

            LOG_DISPLAY(
                LogWorldGameMode,
                "Finished spawning minimum required presistent chunks at player start location (on client and server). Spawning their character."
            )
            PlayerController->SpawnCharacterToWorld();

            continue;
        }
    }

    return;
}
