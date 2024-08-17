// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemLitSv.h"

#include "Player/WorldPlayerController.h"
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
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: Cl.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemDedSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: DedSv.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemStandalone>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: Standalone.")
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

    if (this->ChunkGenerationSubsystem->GetPendingKillVerticalChunkQueue().IsEmpty() == false)
    {
        LOG_ERROR(LogChunkValidation, "Pending kill vertical chunks is not empty.")
    }

    this->ChunkGenerationSubsystem->ClearVerticalChunkQueue();

    TArray<FChunkKey2> PreferredChunks = TArray<FChunkKey2>();
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
                Validation::GetAllChunksInDistance(WorldStatics::WorldToVerticalChunkKey(PredictedLocation), RenderDistance)
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

                if (this->ChunkGenerationSubsystem->HasPersistentVerticalChunk(FChunkKey2(WorldStatics::WorldToVerticalChunkKey(PredictedLocation))) == false)
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
