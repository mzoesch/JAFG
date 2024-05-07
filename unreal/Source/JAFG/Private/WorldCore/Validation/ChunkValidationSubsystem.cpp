// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystem.h"

#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"

UChunkValidationSubsystem::UChunkValidationSubsystem() : Super()
{
    return;
}

void UChunkValidationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<ULocalChunkWorldSettings>();
    Collection.InitializeDependency<UChunkGenerationSubsystem>();
    Super::Initialize(Collection);

    return;
}

void UChunkValidationSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    this->CreateMockChunks();
}

void UChunkValidationSubsystem::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);
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

    UChunkGenerationSubsystem* ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();

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

        for (int Z = ChunksAboveZeroClient; Z >= 0; --Z)
        {
            ChunkGenerationSubsystem->SpawnChunkAsync(FIntVector(0, 0, Z));
        }
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
                for (int Z = ChunksAboveZeroClient; Z >= 0; --Z)
                {
                    ChunkGenerationSubsystem->SpawnChunkAsync(FIntVector(this->TargetPoint.X, this->TargetPoint.Y, Z));
                }

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
