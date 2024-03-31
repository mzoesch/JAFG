// Copyright 2024 mzoesch. All rights reserved.

#include "World/WorldGeneratorInfo.h"

#include "World/Chunk/GreedyChunk.h"

AWorldGeneratorInfo::AWorldGeneratorInfo(const FObjectInitializer& ObjectInitializer)
{
	this->PrimaryActorTick.bCanEverTick = true;
	this->PrimaryActorTick.bStartWithTickEnabled = true;
	this->PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	this->PrimaryActorTick.TickInterval = 1.0f;

	this->bReplicates = false;
	this->bNetLoadOnClient = false;

	this->FullyLoadedChunks = TMap<FIntVector, ACommonChunk*>();
	this->ChunkGenerationQueue.Empty();
	
	return;
}

void AWorldGeneratorInfo::BeginPlay(void)
{
	Super::BeginPlay();

	this->FullyLoadedChunks.Empty();
	this->ChunkGenerationQueue.Empty();

	this->GenerateWorldAsync();

	UE_LOG(LogTemp, Warning, TEXT("AWorldGeneratorInfo::BeginPlay: Enqueued all chunks. Predicted chunks in queue: %d"), this->MaxSpiralPoints * (this->ChunksAboveZero + 1))

	return;
}

void AWorldGeneratorInfo::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->ChunkGenerationQueue.IsEmpty())
	{
		return;
	}

	for (int i = 0; i < 20; ++i)
	{
		FIntVector Key;
		if (this->ChunkGenerationQueue.Dequeue(Key) == false)
		{
			UE_LOG(LogTemp, Log, TEXT("AWorldGeneratorInfo::Tick(): Dequeue failed. World generation is complete for this tick."))
			return;
		}

		const FTransform TargetedChunkTransform = FTransform(
			FRotator::ZeroRotator,
			FVector(
				Key.X * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale,
				Key.Y * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale,
				Key.Z * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale
			),
			FVector::OneVector
		);

		ACommonChunk* Chunk = this->GetWorld()->SpawnActor<ACommonChunk>(AGreedyChunk::StaticClass(), TargetedChunkTransform);

		/* We of course have to first add this to the sweep steps later. */
		this->FullyLoadedChunks.Add(Key, Chunk);

		continue;
	}

	return;
	
}

void AWorldGeneratorInfo::GenerateWorldAsync()
{
	auto MoveCursorRight = [](const FIntVector2& CursorLocation)
	{ return FIntVector2(CursorLocation.X + 1, CursorLocation.Y); };
	auto MoveCursorDown = [](const FIntVector2& CursorLocation)
	{ return FIntVector2(CursorLocation.X, CursorLocation.Y - 1); };
	auto MoveCursorLeft = [](const FIntVector2& CursorLocation)
	{ return FIntVector2(CursorLocation.X - 1, CursorLocation.Y); };
	auto MoveCursorUp = [](const FIntVector2& CursorLocation)
	{ return FIntVector2(CursorLocation.X, CursorLocation.Y + 1); };
	const auto Moves = TArray<FIntVector2(*)(const FIntVector2&)>({MoveCursorRight, MoveCursorDown, MoveCursorLeft, MoveCursorUp});
	int CurrentMoveIndex = 0;

	int n = 1;
	FIntVector2 TargetPoint = FIntVector2(0, 0);
	int TimesToMove = 1;

	for (int Z = this->ChunksAboveZero; Z >= 0; --Z)
	{
		const FIntVector Key = FIntVector(0, 0, Z);
		this->ChunkGenerationQueue.Enqueue(Key);
	}

	while (true)
	{
		for (int _ = 0; _ < 2; ++_)
		{
			CurrentMoveIndex = (CurrentMoveIndex + 1) % Moves.Num();
			for (int __ = 0; __ < TimesToMove; ++__)
			{
				if (n >= this->MaxSpiralPoints)
				{
					return;
				}

				TargetPoint = Moves[CurrentMoveIndex](TargetPoint);

				++n;
				for (int Z = this->ChunksAboveZero; Z >= 0; --Z)
				{
					const FIntVector Key = FIntVector(TargetPoint.X, TargetPoint.Y, Z);
					this->ChunkGenerationQueue.Enqueue(Key);
				}

				continue;
			}

			continue;
		}

		++TimesToMove;
        
		continue;
	}
}
