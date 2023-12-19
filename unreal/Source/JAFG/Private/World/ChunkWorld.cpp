// © 2023 mzoesch. All rights reserved.

#include "World/ChunkWorld.h"

#include "Kismet/GameplayStatics.h"

#include "World/Chunk.h"
#include "World/JCoordinate.h"
#include "World/Chunks/GreedyChunk.h"

AChunkWorld::AChunkWorld()
{
    // Set this actor to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    return;
}

void AChunkWorld::BeginPlay()
{
    Super::BeginPlay();

    this->GenerateWorld();

    return;
}

void AChunkWorld::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
    UE_LOG(LogTemp, Error, TEXT("AChunkWorld::Tick() was called."))
    return;
}

void AChunkWorld::GenerateWorld()
{
    // 1. Generate a biome map.
    // 2. Generate the chunks according to the biome map.

    /* This is ofc very basic and has to be rewritten to generate chunks around a character and destroy them if not. */
    for (int X = -this->DetailedDrawDistance; X <= this->DetailedDrawDistance; X++)
    {
        for (int Y = -this->DetailedDrawDistance; Y <= this->DetailedDrawDistance; Y++)
        {
            for (int Z = -this->ChunksBelowZero; Z <= this->DrawHeight - this->ChunksBelowZero; Z++)
            {
                const FTransform Transform = FTransform(
                    FRotator::ZeroRotator,
                    FVector(
                        X * AChunk::CHUNK_SIZE * AJCoordinate::J_TO_U_SCALE,
                        Y * AChunk::CHUNK_SIZE * AJCoordinate::J_TO_U_SCALE,
                        Z * AChunk::CHUNK_SIZE * AJCoordinate::J_TO_U_SCALE
                    ),
                    FVector::OneVector
                );

                AChunk* Chunk = this->GetWorld()->SpawnActorDeferred<AChunk>(
                    AGreedyChunk::StaticClass(),
                    Transform,
                    this
                );

                /* TODO Here set the biomes etc. maybe? */

                UGameplayStatics::FinishSpawningActor(Chunk, Transform);

                continue;
            }

            continue;
        }

        continue;
    }

    return;
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
