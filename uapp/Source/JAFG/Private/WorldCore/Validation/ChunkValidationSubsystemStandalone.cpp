// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemStandalone.h"

#include "Player/WorldPlayerController.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#include "WorldCore/Validation/ChunkValidationSubsystemCl.h"
#include "WorldCore/Validation/ChunkValidationSubsystemDedSv.h"
#include "WorldCore/Validation/ChunkValidationSubsystemLitSv.h"
#if WITH_EDITOR
    #include "Editor.h"
    #include "LevelEditorViewport.h"
#endif /* WITH_EDITOR */

UChunkValidationSubsystemStandalone::UChunkValidationSubsystemStandalone() : Super()
{
    return;
}

void UChunkValidationSubsystemStandalone::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<ULocalChunkWorldSettings>();
    Collection.InitializeDependency<UChunkGenerationSubsystem>();
    Super::Initialize(Collection);

    LOG_DISPLAY(LogChunkValidation, "Called.")

    this->SetTickInterval(2.0f);

    return;
}

bool UChunkValidationSubsystemStandalone::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

#if WITH_EDITOR
    return UNetStatics::IsSafeStandalone(Outer);
#else /* WITH_EDITOR */
    return UNetStatics::IsSafeStandaloneNoServer(Outer);
#endif /* !WITH_EDITOR */
}

void UChunkValidationSubsystemStandalone::OnWorldBeginPlay(UWorld& InWorld)
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
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemLitSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: LitSv.")
    }

    this->ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( this->ChunkGenerationSubsystem )

    return;
}

void UChunkValidationSubsystemStandalone::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    AWorldPlayerController* LocalPlayerController = this->GetLocalPlayerController<AWorldPlayerController>();

    FVector PredictedLocation;
#if WITH_EDITOR
    if (GEditor && GEditor->IsSimulateInEditorInProgress())
    {
        PredictedLocation = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation();
    }
#else /* WITH_EDITOR */
    if (false)
    {
    }
#endif /* !WITH_EDITOR */
    else
    {
        check( LocalPlayerController )
        if (LocalPlayerController->GetPredictedCharacterLocation(PredictedLocation) == false)
        {
            return;
        }
    }

    this->LoadUnloadChunks(PredictedLocation);

    if (LocalPlayerController == nullptr || LocalPlayerController->HasSuccessfullySpawnedCharacter())
    {
        return;
    }

    if (this->ChunkGenerationSubsystem->HasPersistentVerticalChunk(FChunkKey2(ChunkStatics::WorldToVerticalChunkKey(PredictedLocation))) == false)
    {
        return;
    }

    LOG_DISPLAY(LogWorldGameMode, "Finished spawning minimum required presistent chunks at player start location. Spawning character.")
    LocalPlayerController->SpawnCharacterToWorld();

    return;
}

void UChunkValidationSubsystemStandalone::LoadUnloadChunks(const FVector& LocalPlayerLocation) const
{
    constexpr int RenderDistance { 5 };

    if (this->ChunkGenerationSubsystem->GetPendingKillVerticalChunkQueue().IsEmpty() == false)
    {
        LOG_ERROR(LogChunkValidation, "Pending kill vertical chunks is not empty.")
    }

    this->ChunkGenerationSubsystem->ClearVerticalChunkQueue();

    TArray<FChunkKey2> PreferredChunks = Validation::GetAllChunksInDistance(ChunkStatics::WorldToVerticalChunkKey(LocalPlayerLocation), RenderDistance);

    // Loading
    //////////////////////////////////////////////////////////////////////////
    int32 NewChunksCounter = 0;
    for (const FChunkKey2& Preferred : PreferredChunks)
    {
        if (this->ChunkGenerationSubsystem->GetVerticalChunks().Contains(Preferred) == false)
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

template<class T>
T* UChunkValidationSubsystemStandalone::GetLocalPlayerController(void) const
{
    return Cast<T>(this->GetWorld()->GetFirstPlayerController());
}
