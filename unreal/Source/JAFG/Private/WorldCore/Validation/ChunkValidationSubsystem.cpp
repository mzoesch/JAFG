// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystem.h"

#include "Editor.h"
#include "LevelEditorViewport.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"

#if !UE_BUILD_SHIPPING
#define CREATE_MOCK_CHUNKS 0
#endif /* !UE_BUILD_SHIPPING */

UChunkValidationSubsystem::UChunkValidationSubsystem() : Super()
{
    return;
}

void UChunkValidationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<ULocalChunkWorldSettings>();
    Collection.InitializeDependency<UChunkGenerationSubsystem>();
    Super::Initialize(Collection);

    this->SetTickInterval(2.0f);

    return;
}

bool UChunkValidationSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    if (UNetStatics::IsSafeDedicatedServer(Outer))
    {
        return false;
    }

    return true;
}

void UChunkValidationSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    this->ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( this->ChunkGenerationSubsystem )

#if !UE_BUILD_SHIPPING && CREATE_MOCK_CHUNKS
    this->CreateMockChunks();
#endif /* !UE_BUILD_SHIPPING && CREATE_MOCK_CHUNKS */

    return;
}

void UChunkValidationSubsystem::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    FVector LocalPlayerLocation;
#if WITH_EDITOR
    if (GEditor->bIsSimulatingInEditor)
    {
        LocalPlayerLocation = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation();
    }
    else
    {
        LocalPlayerLocation = GEngine->GetFirstLocalPlayerController(this->GetWorld())->GetPawnOrSpectator()->GetActorLocation();
    }
#else /* WITH_EDITOR */
    LocalPlayerLocation = GEngine->GetFirstLocalPlayerController(this->GetWorld())->GetPawnOrSpectator()->GetActorLocation();
#endif /* !WITH_EDITOR */

    this->LoadUnloadChunks(LocalPlayerLocation);

    return;
}

void UChunkValidationSubsystem::LoadUnloadChunks(const FVector& LocalPlayerLocation) const
{
    constexpr int RenderDistance { 10 };

    if (this->ChunkGenerationSubsystem->PendingKillVerticalChunks.IsEmpty() == false)
    {
        LOG_ERROR(LogChunkValidation, "Pending kill vertical chunks is not empty.")
    }

    this->ChunkGenerationSubsystem->ActiveVerticalChunksToGenerateAsyncQueue.Empty();

    TArray<FChunkKey2> PreferredChunks = this->GetAllChunksInDistance(ChunkConversion::WorldToVerticalChunkKey(LocalPlayerLocation) , RenderDistance);

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

    LOG_VERY_VERBOSE(LogChunkValidation, "Decided to load %d and unload %d chunks.", NewChunksCounter, UnloadedChunksCounter)

    return;
}

TArray<FChunkKey2> UChunkValidationSubsystem::GetAllChunksInDistance(const FChunkKey2& Center, const int32 Distance)
{
    TArray<FChunkKey2> Out; Out.Reserve((Distance * 2 + 1) * (Distance * 2 + 1));

    for (int X = -Distance; X <= Distance; ++X)
    {
        for (int Y = -Distance; Y <= Distance; ++Y)
        {
            Out.Emplace(Center.X + X, Center.Y + Y);
        }
    }

    return Out;
}

void UChunkValidationSubsystem::CreateMockChunks(void)
{
    if (this->bFinishedMockingChunkGeneration)
    {
        return;
    }

    if (UNetStatics::IsSafeDedicatedServer(this))
    {
        LOG_FATAL(LogChunkValidation, "Disallowed call on server.")
        return;
    }

    // ReSharper disable once CppTooWideScopeInitStatement
    constexpr int ChunksAboveZeroClient {  4 };
    // ReSharper disable once CppTooWideScopeInitStatement
    constexpr int MaxSpiralPointsClient { 40 };

    auto MoveCursorRight = [] (const FIntVector2& CursorLocation)
    {
        return FIntVector2(CursorLocation.X + 1, CursorLocation.Y);
    };

    auto MoveCursorDown = [] (const FIntVector2& CursorLocation)
    {
        return FIntVector2(CursorLocation.X, CursorLocation.Y - 1);
    };

    auto MoveCursorLeft = [] (const FIntVector2& CursorLocation)
    {
        return FIntVector2(CursorLocation.X - 1, CursorLocation.Y);
    };

    auto MoveCursorUp = [] (const FIntVector2& CursorLocation)
    {
        return FIntVector2(CursorLocation.X, CursorLocation.Y + 1);
    };

    const auto Moves = TArray<FIntVector2(*)(const FIntVector2&)>(
    {
        MoveCursorRight, MoveCursorDown, MoveCursorLeft, MoveCursorUp
    });

    int SpiralsAddedThisTick = 0;

    /* First iteration. */
    if (this->MockCursor == 1)
    {
        SpiralsAddedThisTick++;

        // for (int Z = ChunksAboveZeroClient; Z >= 0; --Z)
        // {
        //     ChunkGenerationSubsystem->SpawnActiveChunkAsync(FChunkKey(0, 0, Z));
        // }
        // this->ChunkGenerationSubsystem->SpawnActiveVerticalChunkAsync(FChunkKey2(0, 0));
    }

    while (true)
    {
        for (int _ = 0; _ < 2; ++_)
        {
            this->CurrentMoveIndex = (this->CurrentMoveIndex + 1) % Moves.Num();
            for (int __ = 0; __ < this->TimesToMove; ++__)
            {
                this->TargetPoint = Moves[this->CurrentMoveIndex](this->TargetPoint);

                ++this->MockCursor;
                ++SpiralsAddedThisTick;
                // for (int Z = ChunksAboveZeroClient; Z >= 0; --Z)
                // {
                //     ChunkGenerationSubsystem->SpawnActiveChunkAsync(FIntVector(this->TargetPoint.X, this->TargetPoint.Y, Z));
                // }
                // this->ChunkGenerationSubsystem->SpawnActiveVerticalChunkAsync(FChunkKey2(this->TargetPoint.X, this->TargetPoint.Y));

                continue;
            }

            continue;
        }

        ++this->TimesToMove;

        if (this->MockCursor >= MaxSpiralPointsClient)
        {
            this->bFinishedMockingChunkGeneration = true;
            return;
        }

        continue;
    }

    return;
}

#if !UE_BUILD_SHIPPING
#undef CREATE_MOCK_CHUNKS
#endif /* !UE_BUILD_SHIPPING */
