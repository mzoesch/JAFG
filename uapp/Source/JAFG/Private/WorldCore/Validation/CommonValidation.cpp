// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/CommonValidation.h"
#include "Player/WorldPlayerController.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#if WITH_EDITOR
    #include "Editor.h"
    #include "LevelEditorViewport.h"
#endif /* WITH_EDITOR */

UChunkValidationSubsystemCommon::UChunkValidationSubsystemCommon(void) : Super()
{
    return;
}

void UChunkValidationSubsystemCommon::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<ULocalChunkWorldSettings>();
    Collection.InitializeDependency<UChunkGenerationSubsystem>();
    Super::Initialize(Collection);

    this->SetTickInterval(2.0f);

    return;
}

bool UChunkValidationSubsystemCommon::ShouldCreateSubsystem(UObject* Outer) const
{
    return Super::ShouldCreateSubsystem(Outer);
}

void UChunkValidationSubsystemCommon::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    this->ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( this->ChunkGenerationSubsystem )

    return;
}

void UChunkValidationSubsystemCommon::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);
}

void UChunkValidationSubsystemCommon::LoadUnloadChunks(const TArray<FVector>& WorldLocations, const FChunkLoadingParams& Params) const
{
    if (this->ChunkGenerationSubsystem->GetPendingKillVerticalChunkQueue().IsEmpty() == false)
    {
        LOG_RELAXED_FATAL(LogChunkValidation, "Vertical pendig kill queue is not empty. Aborting.")
        return;
    }

    this->ChunkGenerationSubsystem->ClearVerticalChunkQueue();

    TArray<FChunkKey2> PreferredChunks;
    for (const FVector& Location : WorldLocations)
    {
        for (
            FChunkKey2 Key
                :
            Validation::GetAllChunksInDistance(WorldStatics::WorldToVerticalChunkKey(Location), Params.RenderDistance)
        )
        {
            PreferredChunks.AddUnique(Key);
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
#endif /* !UE_BUILD_SHIPPING */

    return;
}

void UChunkValidationSubsystemCommon::TrySpawnLocalCharacter(void) const
{
    AWorldPlayerController* LocalPlayerController = this->GetLocalPlayerController<AWorldPlayerController>();
    if (LocalPlayerController == nullptr || LocalPlayerController->HasSuccessfullySpawnedCharacter())
    {
        return;
    }

    check( LocalPlayerController->IsLocalController() )

    FVector PredictedLocation;
    if (this->GetPredictedLocalPlayerLocation(PredictedLocation) == false)
    {
        return;
    }

    if (this->ChunkGenerationSubsystem->HasPersistentVerticalChunk(FChunkKey2(WorldStatics::WorldToVerticalChunkKey(PredictedLocation))) == false)
    {
        return;
    }

    if (UNetStatics::IsSafeClient(this))
    {
        if (LocalPlayerController->IsClientReadyForCharacterSpawn())
        {
            checkNoEntry()
        }

        LOG_DISPLAY(
            LogWorldGameMode,
            "Finished spawning minimum required presistent chunks at player start location. Giving ok for character spawn to server."
        )
        LocalPlayerController->SetClientReadyForCharacterSpawn();
    }
    else
    {
        LOG_DISPLAY(
            LogWorldGameMode,
            "Finished spawning minimum required presistent chunks at player start location. Spawning character."
        )
        LocalPlayerController->SpawnCharacterToWorld();
    }

    return;
}

void UChunkValidationSubsystemCommon::TrySpawnRemoteCharacters(void) const
{
    check( UNetStatics::IsSafeListenServer(this) || UNetStatics::IsSafeDedicatedServer(this) )

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

bool UChunkValidationSubsystemCommon::GetPredictedLocalPlayerLocation(FVector& Out) const
{
#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        Out = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation();
        return true;
    }
#endif /* WITH_EDITOR */

    const AWorldPlayerController* LocalPlayerController = this->GetLocalPlayerController<AWorldPlayerController>();
    if (LocalPlayerController == nullptr)
    {
        return false;
    }

    return LocalPlayerController->GetPredictedCharacterLocation(Out);
}

TArray<FVector> UChunkValidationSubsystemCommon::GetAllPredictedPlayerLocations(void) const
{
    TArray<FVector> Out;

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

            Out.Add(PredictedLocation);

            continue;
        }
    }

    return Out;
}

template<class T>
T* UChunkValidationSubsystemCommon::GetLocalPlayerController(void) const
{
    return Cast<T>(this->GetWorld()->GetFirstPlayerController());
}
