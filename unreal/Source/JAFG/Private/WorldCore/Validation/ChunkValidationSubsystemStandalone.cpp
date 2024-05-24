// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemStandalone.h"

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

    if (GEngine->GetFirstLocalPlayerController(this->GetWorld())->GetPawnOrSpectator() == nullptr)
    {
        return;
    }

#if WITH_EDITOR
    if (GEditor->IsSimulateInEditorInProgress())
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
