// © 2023 mzoesch. All rights reserved.

#include "World/ChunkWorld.h"

#include "SWarningOrErrorBox.h"
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
    // Set this actor to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    this->PrimaryActorTick.bCanEverTick = false;
    this->LoadedChunks = TMap<FIntVector, AChunk*>();

    this->NContinentalness = new FastNoiseLite();
    
    return;
}

void AChunkWorld::BeginPlay()
{
    Super::BeginPlay();

    this->LoadedChunks.Empty();

    this->NContinentalness->SetSeed(this->Seed);
    this->NContinentalness->SetFrequency(this->ContinentalnessFrequency);
    this->NContinentalness->SetFractalType(this->ContinentalnessFractalType);
    this->NContinentalness->SetNoiseType(this->ContinentalnessNoiseType);

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

                this->LoadedChunks.Add(FIntVector(X, Y, Z), Chunk);
                
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

float AChunkWorld::GetDensity(const float X, const float Y, const float Z) const
{
    const float PercentHeight = (Z - this->GetLowestPoint()) / (this->GetHighestPoint() - this->GetLowestPoint()) * 100.0f;
    
    // const float Density = 2.0 * (PercentHeight / 100.0) - 1.0;

    const float Density = FNoiseSplinePoint::GetDensity(this->NoiseSplinePoints, PercentHeight, X, Y);
    
    return Density;
}

float AChunkWorld::GenDensity(const float ContinentalnessNoise, const float WorldZ)
{



    return 0.0f;
}
