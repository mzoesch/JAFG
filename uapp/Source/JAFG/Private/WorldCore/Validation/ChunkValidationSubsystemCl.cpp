// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemCl.h"

#include "Network/MyHyperlaneComponent.h"
#include "Player/WorldPlayerController.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#include "WorldCore/Validation/ChunkValidationSubsystemDedSv.h"
#include "WorldCore/Validation/ChunkValidationSubsystemLitSv.h"
#include "WorldCore/Validation/ChunkValidationSubsystemStandalone.h"

UChunkValidationSubsystemCl::UChunkValidationSubsystemCl() : Super()
{
    return;
}

void UChunkValidationSubsystemCl::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<ULocalChunkWorldSettings>();
    Super::Initialize(Collection);

    LOG_DISPLAY(LogChunkValidation, "Called.")

    this->SetTickInterval(2.0f);

    return;
}

bool UChunkValidationSubsystemCl::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return UNetStatics::IsSafeClient(Outer);
}

void UChunkValidationSubsystemCl::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemDedSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: DedSv.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemLitSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: LitSv.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemStandalone>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: Standalone.")
    }

    this->ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( this->ChunkGenerationSubsystem )

    return;
}

void UChunkValidationSubsystemCl::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    const AWorldPlayerController* LocalController = this->GetLocalPlayerController<AWorldPlayerController>();

    if (LocalController == nullptr)
    {
        return;
    }

    if (LocalController->GetComponentByClass<UMyHyperlaneComponent>()->IsConnectedAndReady() == false)
    {
        return;
    }

    FVector PredictedLocation;
    if (LocalController->GetPredictedCharacterLocation(PredictedLocation) == false)
    {
        return;
    }

    this->LoadUnLoadChunks(PredictedLocation);

    return;
}

void UChunkValidationSubsystemCl::LoadUnLoadChunks(const FVector& LocalPlayerLocation) const
{
    constexpr int RenderDistance { 2 /*10*/ };

    if (this->ChunkGenerationSubsystem->IsReady() == false)
    {
        return;
    }

    if (this->ChunkGenerationSubsystem->GetPendingKillVerticalChunkQueue().IsEmpty() == false)
    {
        LOG_ERROR(LogChunkValidation, "Pending kill vertical chunks is not empty.")
    }

    this->ChunkGenerationSubsystem->ClearVerticalChunkQueue();

    TArray<FChunkKey2> PreferredChunks = Validation::GetAllChunksInDistance(WorldStatics::WorldToVerticalChunkKey(LocalPlayerLocation), RenderDistance);

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

    AWorldPlayerController* PlayerController = this->GetLocalPlayerController<AWorldPlayerController>();

    if (PlayerController->IsClientReadyForCharacterSpawn())
    {
        return;
    }

    FVector PredictedLocation;
    if (PlayerController->GetPredictedCharacterLocation(PredictedLocation) == false)
    {
        return;
    }

    if (this->ChunkGenerationSubsystem->HasPersistentVerticalChunk(FChunkKey2(WorldStatics::WorldToVerticalChunkKey(PredictedLocation))) == false)
    {
        return;
    }

    PlayerController->SetClientReadyForCharacterSpawn();

    return;
}

template<class T>
T* UChunkValidationSubsystemCl::GetLocalPlayerController(void) const
{
    return Cast<T>(this->GetWorld()->GetFirstPlayerController());
}
