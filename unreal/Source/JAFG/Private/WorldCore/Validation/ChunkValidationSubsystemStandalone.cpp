// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemStandalone.h"

#include "Editor.h"
#include "LevelEditorViewport.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#include "WorldCore/Validation/ChunkValidationSubsystemCl.h"
#include "WorldCore/Validation/ChunkValidationSubsystemDedSv.h"
#include "WorldCore/Validation/ChunkValidationSubsystemLitSv.h"

UChunkValidationSubsystemStandalone::UChunkValidationSubsystemStandalone() : Super()
{
    return;
}

void UChunkValidationSubsystemStandalone::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<ULocalChunkWorldSettings>();
    Collection.InitializeDependency<UChunkGenerationSubsystem>();
    Super::Initialize(Collection);

    this->SetTickInterval(2.0f);

    return;
}

bool UChunkValidationSubsystemStandalone::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return UNetStatics::IsSafeStandalone(Outer);
}

void UChunkValidationSubsystemStandalone::OnWorldBeginPlay(UWorld& InWorld)
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
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemLitSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystems: LitSv.")
    }

    this->ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( this->ChunkGenerationSubsystem )

    return;
}

void UChunkValidationSubsystemStandalone::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

#if WITH_EDITOR
    if (GEditor->bIsSimulatingInEditor)
    {
        this->LoadUnloadChunks(GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation());
    }
    else
    {
        this->LoadUnloadChunks(GEngine->GetFirstLocalPlayerController(this->GetWorld())->GetPawnOrSpectator()->GetActorLocation());
    }
#else /* WITH_EDITOR */
    this->LoadUnloadChunks(GEngine->GetFirstLocalPlayerController(this->GetWorld())->GetPawnOrSpectator()->GetActorLocation());
#endif /* !WITH_EDITOR */

    return;
}

void UChunkValidationSubsystemStandalone::LoadUnloadChunks(const FVector& LocalPlayerLocation) const
{
    constexpr int RenderDistance { 10 };

    if (this->ChunkGenerationSubsystem->PendingKillVerticalChunks.IsEmpty() == false)
    {
        LOG_ERROR(LogChunkValidation, "Pending kill vertical chunks is not empty.")
    }

    this->ChunkGenerationSubsystem->ActiveVerticalChunksToGenerateAsyncQueue.Empty();

    TArray<FChunkKey2> PreferredChunks = Validation::GetAllChunksInDistance(ChunkConversion::WorldToVerticalChunkKey(LocalPlayerLocation), RenderDistance);

    // Loading
    //////////////////////////////////////////////////////////////////////////
    int32 NewChunksCounter = 0;
    for (const FChunkKey2& Preferred : PreferredChunks)
    {
        if (this->ChunkGenerationSubsystem->ActiveVerticalChunks.Contains(Preferred) == false)
        {
            this->ChunkGenerationSubsystem->ActiveVerticalChunksToGenerateAsyncQueue.Enqueue(Preferred);
            NewChunksCounter++;
        }
    }

    // Unloading
    //////////////////////////////////////////////////////////////////////////
    int32 UnloadedChunksCounter = 0;
    for (const FChunkKey2& ActiveChunk : this->ChunkGenerationSubsystem->ActiveVerticalChunks)
    {
        if (PreferredChunks.Contains(ActiveChunk) == false)
        {
            this->ChunkGenerationSubsystem->AddVerticalChunkToKillQueue(ActiveChunk);
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
