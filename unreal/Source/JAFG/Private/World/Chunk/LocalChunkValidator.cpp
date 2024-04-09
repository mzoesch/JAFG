// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/LocalChunkValidator.h"

#include "Kismet/GameplayStatics.h"
#include "Network/NetworkStatics.h"
#include "World/Chunk/LocalPlayerChunkGeneratorSubsystem.h"
#include "World/WorldGeneratorInfo.h"
#include "World/WorldPlayerController.h"
#include "World/Chunk/GreedyChunk.h"

#define OWNING_PAWN Cast<APawn>(this->GetOwner())
#define CHECK_OWNING_PAWN check( this->GetOwner() ) check( Cast<APawn>(this->GetOwner()) )
#define CHECKED_OWNING_PAWN CHECK_OWNING_PAWN OWNING_PAWN

ULocalChunkValidator::ULocalChunkValidator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = true;
    this->PrimaryComponentTick.TickInterval = 1.0f;
}

void ULocalChunkValidator::BeginPlay(void)
{
    Super::BeginPlay();

    CHECK_OWNING_PAWN

    if (UNetworkStatics::IsSafeClient(this) && OWNING_PAWN->IsLocallyControlled())
    {
        LOG_VERBOSE(LogChunkValidation, "Spawned on a client. Activating.")

        this->SetComponentTickEnabled(true);

        check( GEngine )
        check( this->GetWorld() )
        const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(this->GetWorld());
        check( LocalPlayer )
        this->ChunkGeneratorSubsystem = LocalPlayer->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>();
        check( this->ChunkGeneratorSubsystem )
        this->ChunkGeneratorSubsystem->LoadedChunks.Empty();

        this->WorldGeneratorInfo = nullptr;

        return;
    }


    if (UNetworkStatics::IsSafeListenServer(this) && OWNING_PAWN->IsLocallyControlled())
    {
        LOG_VERBOSE(LogChunkValidation, "Spawned on a listen server. Activating.")

        this->SetComponentTickEnabled(true);

        check( GEngine )
        check( this->GetWorld() )
        const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(this->GetWorld());
        check( LocalPlayer )
        this->ChunkGeneratorSubsystem = LocalPlayer->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>();
        check( this->ChunkGeneratorSubsystem )
        this->ChunkGeneratorSubsystem->LoadedChunks.Empty();

        AActor* _ = UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass());
        check( _ )
        this->WorldGeneratorInfo = Cast<AWorldGeneratorInfo>(_);
        check( this->WorldGeneratorInfo )

        return;
    }

    if (UNetworkStatics::IsSafeServer(this))
    {
        LOG_VERBOSE(LogChunkValidation, "Spawned on a dedicated server or is not locally controlled. Initializing minimum variables. Deactivating.")

        this->SetComponentTickEnabled(false);

        this->ChunkGeneratorSubsystem = nullptr;

        AActor* _ = UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass());
        check( _ )
        this->WorldGeneratorInfo = Cast<AWorldGeneratorInfo>(_);
        check( this->WorldGeneratorInfo )

        return;
    }

    LOG_VERBOSE(LogChunkValidation, "Spawned on a non locally controlled client. Deactivating.")

    this->SetComponentTickEnabled(false);
    this->ChunkGeneratorSubsystem = nullptr;
    this->WorldGeneratorInfo = nullptr;

    return;
}

void ULocalChunkValidator::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    LOG_VERY_VERBOSE(LogChunkValidation, "Validating local chunks.")

    this->GenerateMockChunks();

    return;
}

bool ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC_Validate(const FIntVector& ChunkKey)
{
    if (this->ChunkHandles.Contains(ChunkKey))
    {
        CHECK_OWNING_PAWN
        LOG_ERROR(LogChunkValidation, "Chunk already requested by this (%s) client.", *OWNING_PAWN->GetName())
        return false;
    }

    return true;
}

void ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC_Implementation(const FIntVector& ChunkKey)
{
    check( this->WorldGeneratorInfo )

    if (this->WorldGeneratorInfo->HasFullyLoadedChunk(ChunkKey))
    {
        LOG_FATAL(LogChunkValidation, "Chunk already loaded. Not implemented yet.")
        return;
    }

    const FDelegateHandle Handle = this->WorldGeneratorInfo->OnChunkFinishedGeneratingDelegate.AddLambda( [&, ChunkKey] (const ACommonChunk* Chunk)
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

        if (this->ChunkHandles.Contains(ChunkKey) == false)
        {
            LOG_FATAL(LogChunkValidation, "Chunk handle not found for %s.", *ChunkKey.ToString())
            return;
        }

        if (this->WorldGeneratorInfo->OnChunkFinishedGeneratingDelegate.Remove(this->ChunkHandles[ChunkKey]) == false)
        {
            LOG_FATAL(LogChunkValidation, "Error while removing delegate handle for %s.", *ChunkKey.ToString())
            return;
        }

        this->ChunkHandles.Remove(ChunkKey);

        CHECK_OWNING_PAWN

        if (UNetworkStatics::IsSafeListenServer(this) && OWNING_PAWN->IsLocallyControlled())
        {
            LOG_VERBOSE(LogChunkValidation, "Delegate cleanup done for %s. Discarding authority data transmission as this chunk is already loaded (listen server).",*ChunkKey.ToString())
            return;
        }

        LOG_VERBOSE(LogChunkValidation, "Chunk %s finished generating. Asking the Hyperlane Transmitter to give the authority data to the asking client.", *Chunk->GetChunkKey().ToString())
        check( this->GetOwner() )
        check( Cast<APawn>(this->GetOwner()) )
        check( Cast<APawn>(this->GetOwner())->Controller )
        Chunk->SendInitializationDataToClient(Cast<AWorldPlayerController>(Cast<APawn>(this->GetOwner())->Controller));

        return;
    });

    this->ChunkHandles.Add(ChunkKey, Handle);

    if (this->WorldGeneratorInfo->AddChunkToGenerationQueue(ChunkKey) == false)
    {
        LOG_FATAL(LogChunkValidation, "Unknown error while adding item [%s] to chunk generation queue.", *ChunkKey.ToString())
        return;
    }

    LOG_VERBOSE(LogChunkValidation, "Chunk %s requested. Waiting for generation delegate to be called.", *ChunkKey.ToString())

    return;
}

void ULocalChunkValidator::SafeSpawnChunk(const FIntVector& ChunkKey)
{
    LOG_VERBOSE(LogChunkValidation, "Asking to spawn chunk %s.", *ChunkKey.ToString())

    CHECK_OWNING_PAWN

    if (UNetworkStatics::IsSafeDedicatedServer(this) || OWNING_PAWN->IsLocallyControlled() == false)
    {
        LOG_FATAL(LogChunkValidation, "Disallowed call. SV: %s; LC: %s.",
            UNetworkStatics::IsSafeServer(this) ? TEXT("true") : TEXT("false"),
            OWNING_PAWN->IsLocallyControlled() ? TEXT("true") : TEXT("false"))
        return;
    }

    if (UNetworkStatics::IsSafeListenServer(this))
    {
        /*
         * This should be.
         * We do not spawn here. But we directly ask the server RPC implementation for that.
         */

        if (this->AskServerToSpawnChunk_ServerRPC_Validate(ChunkKey) == false)
        {
            LOG_FATAL(LogChunkValidation, "Remote Procedure Call validation failed.")
            return;
        }

        this->AskServerToSpawnChunk_ServerRPC_Implementation(ChunkKey);

        return;
    }

    LOG_ERROR(LogChunkValidation, "Not implemented yet for a client.")
    return;
}

void ULocalChunkValidator::GenerateMockChunks(void)
{
    if (this->bFinishedMockingChunkGeneration)
    {
        return;
    }

    // ReSharper disable once CppTooWideScopeInitStatement
    constexpr int MaxSpiralPoints = 5;
    constexpr int ChunksAboveZero = 2;
    constexpr int MaxPerTick = 20;
    int AddedThisTick = 0;

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

        this->SafeSpawnChunk(Key);

        AddedThisTick++;
        this->MockChunksAdded++;
        if (AddedThisTick >= MaxPerTick)
        {
            return;
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
                    // if (ChunkGeneratorSubsystem->LoadedChunks.Contains(Key) == false)
                    // {
                    //     const FTransform TargetedChunkTransform = FTransform(
                    //         FRotator::ZeroRotator,
                    //         FVector(
                    //             Key.X * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale,
                    //             Key.Y * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale,
                    //             Key.Z * AWorldGeneratorInfo::ChunkSize * AWorldGeneratorInfo::JToUScale
                    //         ),
                    //         FVector::OneVector
                    //     );
                    //
                    //     ACommonChunk* Chunk = this->GetWorld()->SpawnActor<ACommonChunk>(AGreedyChunk::StaticClass(), TargetedChunkTransform);
                    //
                    //     ChunkGeneratorSubsystem->LoadedChunks.Add(Key, Chunk);
                    //     this->AskServerToSpawnChunk_ServerRPC(Key);
                    //

                    this->SafeSpawnChunk(Key);

                    AddedThisTick++;
                    this->MockChunksAdded++;

                    if (AddedThisTick >= MaxPerTick)
                    {
                        return;
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

#undef OWNING_PAWN
#undef CHECK_OWNING_PAWN
#undef CHECKED_OWNING_PAWN
