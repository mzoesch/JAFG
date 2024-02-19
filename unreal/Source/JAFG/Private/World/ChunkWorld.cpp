// © 2023 mzoesch. All rights reserved.

#include "World/ChunkWorld.h"

#include "SWarningOrErrorBox.h"
#include "Assets/General.h"
#include "Kismet/GameplayStatics.h"

#include "Lib/FastNoiseLite.h"
#include "World/JCoordinate.h"
#include "World/Chunks/GreedyChunk.h"

float FNoiseSplinePoint::GetDensity(const TArray<FNoiseSplinePoint>& Points, const float PercentHeight, const float X, const float Y)
{
    // Get the two points that are left and right of the current height.
    FNoiseSplinePoint LeftPoint;;
    FNoiseSplinePoint RightPoint;
    for (int i = 1; i < Points.Num(); ++i)
    {
        if (Points[i].PercentHeight >= PercentHeight)
        {
            LeftPoint = Points[i - 1];
            RightPoint = Points[i];
            break;
        }
        
        continue;
    }

    const float Distance = (PercentHeight - LeftPoint.PercentHeight) / (RightPoint.PercentHeight - LeftPoint.PercentHeight);
    
    const float Density = ( (1 - Distance) * LeftPoint.TargetDensity ) + ( Distance * RightPoint.TargetDensity );


    if (X == 0 && Y == 0) UE_LOG(LogTemp, Warning, TEXT("Height: %f Density: %f Distance %f [{%f %f}{%f %f}]"), PercentHeight, Density, Distance, LeftPoint.PercentHeight, LeftPoint.TargetDensity, RightPoint.PercentHeight, RightPoint.TargetDensity)
    
    return Density;
}

AChunkWorld::AChunkWorld()
{
    PrimaryActorTick.bCanEverTick     = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.bAllowTickOnDedicatedServer = true;
    PrimaryActorTick.TickInterval = 0.5f;
    
    this->LoadedChunks                      = TMap<FIntVector, AChunk*>();
    this->ChunkGenerationQueue.Empty();
    
    this->NWorld                            = new FastNoiseLite();
    this->NContinentalness                  = new FastNoiseLite();
    this->NTree                             = new FastNoiseLite();
    
    return;
}

void AChunkWorld::BeginPlay()
{
    Super::BeginPlay();

    this->LoadedChunks.Empty();
    this->ChunkGenerationQueue.Empty();

    this->NWorld->SetSeed(this->Seed);
    this->NWorld->SetFrequency(this->WorldFrequency);
    this->NWorld->SetFractalType(this->WorldFractalType);
    this->NWorld->SetNoiseType(this->WorldNoiseType);
    
    this->NContinentalness->SetSeed(this->Seed);
    this->NContinentalness->SetFrequency(this->ContinentalnessFrequency);
    this->NContinentalness->SetFractalType(this->ContinentalnessFractalType);
    this->NContinentalness->SetNoiseType(this->ContinentalnessNoiseType);

    this->NTree->SetSeed(this->Seed);
    this->NTree->SetFrequency(this->TreeFrequency);
    this->NTree->SetFractalType(this->TreeFractalType);
    this->NTree->SetNoiseType(this->TreeNoiseType);
    
    this->GenerateWorldAsync();

    return;
}

void AChunkWorld::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (this->ChunkGenerationQueue.IsEmpty())
    {
        return;
    }

    for (int i = 0; i < 20; ++i)
    {
        FIntVector Key;
        this->ChunkGenerationQueue.Dequeue(Key);

        const FTransform Transform = FTransform(
            FRotator::ZeroRotator,
            FVector(
                Key.X * AChunk::CHUNK_SIZE * UJCoordinate::J_TO_U_SCALE,
                Key.Y * AChunk::CHUNK_SIZE * UJCoordinate::J_TO_U_SCALE,
                Key.Z * AChunk::CHUNK_SIZE * UJCoordinate::J_TO_U_SCALE
            ),
            FVector::OneVector
        );

        AChunk* Chunk = this->GetWorld()->SpawnActor<AChunk>(AGreedyChunk::StaticClass(), Transform);

        this->LoadedChunks.Add(Key, Chunk);

        continue;
    }
    
    return;
}

void AChunkWorld::GenerateWorldAsync()
{
    auto MoveRight = [] (const FIntVector2& Position)
    {
        return FIntVector2(Position.X + 1, Position.Y);
    };

    auto MoveDown = [] (const FIntVector2& Position)
    {
        return FIntVector2(Position.X, Position.Y - 1);
    };

    auto MoveLeft = [] (const FIntVector2& Position)
    {
        return FIntVector2(Position.X - 1, Position.Y);
    };

    auto MoveUp = [] (const FIntVector2& Position)
    {
        return FIntVector2(Position.X, Position.Y + 1);
    };
    
    const auto Moves = TArray<FIntVector2(*)(const FIntVector2&)>({MoveRight, MoveDown, MoveLeft, MoveUp});
    int MoveIdx = 0;

    int n = 1;
    FIntVector2 TargetPoint = FIntVector2(0, 0);
    int TimesToMove = 1;

    for (int Z = this->ChunksAboveZero; Z >= 0; --Z)
    {
        const FIntVector Key = FIntVector(0, 0, Z);
        this->ChunkGenerationQueue.Enqueue(Key);

        continue;
    }

    while (true)
    {
        for (int _ = 0; _ < 2; ++_)
        {
            MoveIdx = (MoveIdx + 1) % Moves.Num();
            for (int __ = 0; __ < TimesToMove; ++__)
            {
                if (n >= this->MaxSpiralPoints)
                {
                    return;
                }

                TargetPoint = Moves[MoveIdx](TargetPoint);
                
                n++;
                for (int Z = this->ChunksAboveZero; Z >= 0; --Z)
                {
                    const FIntVector Key = FIntVector(TargetPoint.X, TargetPoint.Y, Z);
                    this->ChunkGenerationQueue.Enqueue(Key);
                    continue;
                }
                
               
            }


            continue;
        }

        TimesToMove++;

        continue;
    }

    UE_LOG(LogTemp, Warning, TEXT("Queued all chunks."))
    
    return;
}

void AChunkWorld::GenerateWorld()
{
    /* This is ofc very basic and has to be rewritten to generate chunks around a character and destroy them if not. */

    // for (int X = -this->DetailedDrawDistance; X <= this->DetailedDrawDistance; ++X)
    // {
    //     for (int Y = -this->DetailedDrawDistance; Y <= this->DetailedDrawDistance; ++Y)
    //     {
    //         for (int Z = this->ChunksAboveZero; Z >= 0; --Z)
    //         {
    //             const FTransform Transform = FTransform(
    //                 FRotator::ZeroRotator,
    //                 FVector(
    //                     X * AChunk::CHUNK_SIZE * AJCoordinate::J_TO_U_SCALE,
    //                     Y * AChunk::CHUNK_SIZE * AJCoordinate::J_TO_U_SCALE,
    //                     Z * AChunk::CHUNK_SIZE * AJCoordinate::J_TO_U_SCALE
    //                 ),
    //                 FVector::OneVector
    //             );
    //
    //             AChunk* Chunk = this->GetWorld()->SpawnActor<AChunk>(AGreedyChunk::StaticClass(), Transform);
    //             
    //             this->LoadedChunks.Add(FIntVector(X, Y, Z), Chunk);
    //             
    //             continue;
    //         }
    //
    //         continue;
    //     }
    //
    //     continue;
    // }
    //
    // UE_LOG(LogTemp, Warning, TEXT("Loaded Chunks: %d"), this->LoadedChunks.Num())
    
    return;
}

AChunk* AChunkWorld::GetChunkByKey(const FIntVector& Key) const
{
    if (this->LoadedChunks.Contains(Key))
    {
        return this->LoadedChunks[Key];
    }
    
    return nullptr;
}

FIntVector AChunkWorld::WorldToChunkPosition(const FVector& WorldPosition)
{
    FIntVector          ChunkPosition;
    const FIntVector    IntPosition     = FIntVector(WorldPosition);
    constexpr int       Factor          = AChunk::CHUNK_SIZE * UJCoordinate::J_TO_U_SCALE;
    
    if (IntPosition.X < 0) ChunkPosition.X = (int) (WorldPosition.X / Factor) - 1;
    else ChunkPosition.X = (int) (WorldPosition.X / Factor);

    if (IntPosition.Y < 0) ChunkPosition.Y = (int) (WorldPosition.Y / Factor) - 1;
    else ChunkPosition.Y = (int) (WorldPosition.Y / Factor);

    if (IntPosition.Z < 0) ChunkPosition.Z = (int) (WorldPosition.Z / Factor) - 1;
    else ChunkPosition.Z = (int) (WorldPosition.Z / Factor);

    return ChunkPosition;
}

FIntVector AChunkWorld::WorldToWorldVoxelPosition(const FVector& WorldPosition)
{
    FIntVector WorldToVoxelPosition = FIntVector(WorldPosition) / 100;

    if (WorldPosition.X < 0) WorldToVoxelPosition.X--;
    if (WorldPosition.Y < 0) WorldToVoxelPosition.Y--;
    if (WorldPosition.Z < 0) WorldToVoxelPosition.Z--;

    return WorldToVoxelPosition;
}

FIntVector AChunkWorld::WorldToLocalVoxelPosition(const FVector& WorldPosition)
{
    /* WorldToChunkPosition */
    FIntVector WorldToChunkPosition;

    const int Factor = AChunk::CHUNK_SIZE * 100;
    const auto IntPosition = FIntVector(WorldPosition);

    if (IntPosition.X < 0) WorldToChunkPosition.X = (int) (WorldPosition.X / Factor) - 1;
    else WorldToChunkPosition.X = (int) (WorldPosition.X / Factor);

    if (IntPosition.Y < 0) WorldToChunkPosition.Y = (int) (WorldPosition.Y / Factor) - 1;
    else WorldToChunkPosition.Y = (int) (WorldPosition.Y / Factor);

    if (IntPosition.Z < 0) WorldToChunkPosition.Z = (int) (WorldPosition.Z / Factor) - 1;
    else WorldToChunkPosition.Z = (int) (WorldPosition.Z / Factor);

    /* WorldToBlockPosition */
    FIntVector WorldToBlockPosition = FIntVector(WorldPosition) / 100 - WorldToChunkPosition * AChunk::CHUNK_SIZE;

    /* Negative Normalization */
    if (WorldToChunkPosition.X < 0) WorldToBlockPosition.X--;
    if (WorldToChunkPosition.Y < 0) WorldToBlockPosition.Y--;
    if (WorldToChunkPosition.Z < 0) WorldToBlockPosition.Z--;
    
    return WorldToBlockPosition;
}
