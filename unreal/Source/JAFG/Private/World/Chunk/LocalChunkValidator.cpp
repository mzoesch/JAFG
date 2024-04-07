// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/LocalChunkValidator.h"

#include "Kismet/GameplayStatics.h"
#include "Network/NetworkStatics.h"
#include "World/Chunk/LocalPlayerChunkGeneratorSubsystem.h"
#include "World/WorldGeneratorInfo.h"
#include "World/WorldPlayerController.h"
#include "World/Chunk/GreedyChunk.h"

ULocalChunkValidator::ULocalChunkValidator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = true;
    this->PrimaryComponentTick.TickInterval = 1.0f;
}

void ULocalChunkValidator::BeginPlay(void)
{
    Super::BeginPlay();

    check( this->GetOwner() && Cast<APawn>(this->GetOwner()) && "ULocalChunkValidator::BeginPlay: Owner is not a pawn." )
    if (UNetworkStatics::IsSafeClient(this) && Cast<APawn>(this->GetOwner())->IsLocallyControlled())
    {
        this->SetComponentTickEnabled(true);

        check( GEngine )
        check( this->GetWorld() )
        ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(this->GetWorld());
        check( LocalPlayer )
        ULocalPlayerChunkGeneratorSubsystem* ChunkGeneratorSubsystem = LocalPlayer->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>();
        check( ChunkGeneratorSubsystem )

        ChunkGeneratorSubsystem->LoadedChunks.Empty();

        return;
    }

    this->SetComponentTickEnabled(false);

    return;
}

void ULocalChunkValidator::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UE_LOG(LogTemp, Log, TEXT("ULocalChunkValidator::TickComponent: Validating local chunks."))

    this->GenerateMockChunks();

    return;
}

bool ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC_Validate(const FIntVector& ChunkKey)
{
    if (this->ChunkHandles.Contains(ChunkKey))
    {
        UE_LOG(LogTemp, Error, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Chunk already requested by this client."))
        return false;
    }

    return true;
}

void ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC_Implementation(const FIntVector& ChunkKey)
{
    AActor* _ = UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass());
    check( _ )
    AWorldGeneratorInfo* WorldGeneratorInfo = Cast<AWorldGeneratorInfo>(_);
    check( WorldGeneratorInfo )

    if (WorldGeneratorInfo->HasFullyLoadedChunk(ChunkKey))
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Chunk already loaded. Not implemented yet.s"))
        return;
    }

    const FDelegateHandle Handle = WorldGeneratorInfo->OnChunkFinishedGeneratingDelegate.AddLambda( [&, WorldGeneratorInfo, ChunkKey] (const ACommonChunk* Chunk)
    {
        check( Chunk )

        if (ChunkKey != Chunk->GetChunkKey())
        {
            /*
             * All deletes are called if any chunk finishes generating.
             * Maybe we can implement a better way to handle this in the future.
             */
            return;
        }

        UE_LOG(LogTemp, Log, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Chunk %s finished generating. Asking the Hyperlane Transmitter to give the authority data to the asking client."), *Chunk->GetChunkKey().ToString())

        if (this->ChunkHandles.Contains(ChunkKey) == false)
        {
            UE_LOG(LogTemp, Fatal, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Chunk handle not found for %s."), *ChunkKey.ToString())
            return;
        }

        if (WorldGeneratorInfo->OnChunkFinishedGeneratingDelegate.Remove(this->ChunkHandles[ChunkKey]) == false)
        {
            UE_LOG(LogTemp, Fatal, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Error while removing delegate handle for %s."), *ChunkKey.ToString())
            return;
        }
        this->ChunkHandles.Remove(ChunkKey);

        check( this->GetOwner() )
        check( Cast<APawn>(this->GetOwner()) )
        check( Cast<APawn>(this->GetOwner())->Controller )
        Chunk->SendInitializationDataToClient(Cast<AWorldPlayerController>(Cast<APawn>(this->GetOwner())->Controller));

        return;
    });

    this->ChunkHandles.Add(ChunkKey, Handle);

    if (WorldGeneratorInfo->AddChunkToGenerationQueue(ChunkKey) == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Error while adding item [%s] to queue."), *ChunkKey.ToString())
        return;
    }

    return;
}

void ULocalChunkValidator::GenerateMockChunks(void)
{
    if (this->bFinishedMockingChunkGeneration)
    {
        return;
    }

    // ReSharper disable once CppTooWideScopeInitStatement
    constexpr int MaxSpiralPoints = 200;
    constexpr int ChunksAboveZero = 5;
    constexpr int MaxPerTick = 20;
    int AddedThisTick = 0;

    check( GEngine )
    check( this->GetWorld() )
    const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(this->GetWorld());
    check( LocalPlayer )
    ULocalPlayerChunkGeneratorSubsystem* ChunkGeneratorSubsystem = LocalPlayer->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>();
    check( ChunkGeneratorSubsystem )

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

    int CurrentMoveIndex = 0;

    int n = 1;
    FIntVector2 TargetPoint = FIntVector2(0, 0);
    int TimesToMove = 1;

    for (int Z = ChunksAboveZero; Z >= 0; --Z)
    {
        const FIntVector Key = FIntVector(0, 0, Z);
        if (ChunkGeneratorSubsystem->LoadedChunks.Contains(Key) == false)
        {
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

            ChunkGeneratorSubsystem->LoadedChunks.Add(Key, Chunk);
            this->AskServerToSpawnChunk_ServerRPC(Key);

            AddedThisTick++;
            this->MockChunksAdded++;

            if (AddedThisTick >= MaxPerTick)
            {
                return;
            }
        }
    }

    while (true)
    {
        for (int _ = 0; _ < 2; ++_)
        {
            CurrentMoveIndex = (CurrentMoveIndex + 1) % Moves.Num();
            for (int __ = 0; __ < TimesToMove; ++__)
            {
                if (n >= MaxSpiralPoints)
                {
                    this->bFinishedMockingChunkGeneration = true;
                    return;
                }

                TargetPoint = Moves[CurrentMoveIndex](TargetPoint);

                ++n;
                for (int Z = ChunksAboveZero; Z >= 0; --Z)
                {
                    const FIntVector Key = FIntVector(TargetPoint.X, TargetPoint.Y, Z);
                    if (ChunkGeneratorSubsystem->LoadedChunks.Contains(Key) == false)
                    {
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

                        ChunkGeneratorSubsystem->LoadedChunks.Add(Key, Chunk);
                        this->AskServerToSpawnChunk_ServerRPC(Key);

                        AddedThisTick++;
                        this->MockChunksAdded++;

                        if (AddedThisTick >= MaxPerTick)
                        {
                            return;
                        }
                    }
                }

                continue;
            }

            continue;
        }

        ++TimesToMove;

        continue;
    }

    return;
}
