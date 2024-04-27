// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/LocalChunkValidator.h"

#include "Kismet/GameplayStatics.h"
#include "Network/HyperlaneWorker.h"
#include "Network/NetworkStatics.h"
#include "World/Chunk/ChunkWorldSubsystem.h"
#include "World/Chunk/LocalPlayerChunkGeneratorSubsystem.h"
#include "World/WorldGeneratorInfo.h"
#include "World/WorldPlayerController.h"
#include "World/Chunk/GreedyChunk.h"

#define OWNING_PAWN                                                                  \
    Cast<APawn>(this->GetOwner())
#define CHECK_OWNING_PAWN                                                            \
    check( this->GetOwner() )                                                        \
    check( Cast<APawn>(this->GetOwner()) )
#define CHECKED_OWNING_PAWN                                                          \
    CHECK_OWNING_PAWN OWNING_PAWN
#define OWNING_CONTROLLER                                                            \
    Cast<AWorldPlayerController>(Cast<APawn>(this->GetOwner())->Controller)
#define CHECK_OWNING_CONTROLLER                                                      \
    check( this->GetOwner() )                                                        \
    check( Cast<APawn>(this->GetOwner()) )                                           \
    check( Cast<APawn>(this->GetOwner())->Controller )                               \
    check( Cast<AWorldPlayerController>(Cast<APawn>(this->GetOwner())->Controller) )

ULocalChunkValidator::ULocalChunkValidator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->PrimaryComponentTick.bCanEverTick = true;
    this->PrimaryComponentTick.TickInterval = 1.0f;

    return;
}

void ULocalChunkValidator::BeginPlay(void)
{
    Super::BeginPlay();

    if (this->GetWorld()->GetName() != TEXT("L_World"))
    {
        LOG_ERROR(LogChunkMisc, "Loaded in a non world level: %s.", *this->GetWorld()->GetName())
        return;
    }

    CHECK_OWNING_PAWN

    if (UNetworkStatics::IsSafeStandalone(this) && OWNING_PAWN->IsLocallyControlled())
    {
        LOG_VERBOSE(LogChunkValidation, "Spawned on a standalone game (Pawn: %s). Activating.", *OWNING_PAWN->GetName())

#if !WITH_EDITOR
        LOG_ERROR(LogChunkValidation, "Standalone game is not supported outside the editor.")
        // CHANGE BACK
        // LOG_FATAL(LogChunkValidation, "Standalone game is not supported outside the editor.")
#endif /* !WITH_EDITOR */

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

        check( this->GetWorld()->GetWorldSettings() )
        this->ChunkWorldSettings = Cast<AChunkWorldSettings>(this->GetWorld()->GetWorldSettings());
        check( this->ChunkWorldSettings )

        return;
    }

    if (UNetworkStatics::IsSafeClient(this) && OWNING_PAWN->IsLocallyControlled())
    {
        LOG_VERBOSE(LogChunkValidation, "Spawned on a client (Pawn %s). Activating.", *OWNING_PAWN->GetName())

        if (OWNING_PAWN->GetName().Contains(TEXT("WorldSimulationSpectatorPawn")))
        {
            LOG_VERBOSE(LogChunkValidation, "Is PIE spectator. Ignoring implementation. Deactivating", *OWNING_PAWN->GetName())
            this->SetComponentTickEnabled(false);
            check( ChunkGeneratorSubsystem == nullptr )
            this->ChunkGeneratorSubsystem = nullptr;
            check( WorldGeneratorInfo == nullptr )
            this->WorldGeneratorInfo = nullptr;
            check( ChunkWorldSettings == nullptr )
            this->ChunkWorldSettings = nullptr;
            return;
        }

        this->SetComponentTickEnabled(true);

        check( GEngine )
        check( this->GetWorld() )
        const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(this->GetWorld());
        check( LocalPlayer )
        this->ChunkGeneratorSubsystem = LocalPlayer->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>();
        check( this->ChunkGeneratorSubsystem )
        this->ChunkGeneratorSubsystem->LoadedChunks.Empty();

        this->WorldGeneratorInfo = nullptr;
        this->ChunkWorldSettings = nullptr;

        return;
    }

    if (UNetworkStatics::IsSafeListenServer(this) && OWNING_PAWN->IsLocallyControlled())
    {
        LOG_VERBOSE(LogChunkValidation, "Spawned on a listen server (Pawn: %s). Activating.", *OWNING_PAWN->GetName())

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

        check( this->GetWorld()->GetWorldSettings() )
        this->ChunkWorldSettings = Cast<AChunkWorldSettings>(this->GetWorld()->GetWorldSettings());
        check( this->ChunkWorldSettings )

        return;
    }

    if (UNetworkStatics::IsSafeServer(this))
    {
        LOG_VERBOSE(LogChunkValidation, "Spawned on a dedicated server or is not locally controlled (Pawn: %s). Initializing minimum variables. Deactivating.", *OWNING_PAWN->GetName())

        this->SetComponentTickEnabled(false);

        this->ChunkGeneratorSubsystem = nullptr;

        AActor* _ = UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass());
        check( _ )
        this->WorldGeneratorInfo = Cast<AWorldGeneratorInfo>(_);
        check( this->WorldGeneratorInfo )

        check( this->GetWorld()->GetWorldSettings() )
        this->ChunkWorldSettings = Cast<AChunkWorldSettings>(this->GetWorld()->GetWorldSettings());
        check( this->ChunkWorldSettings )

        return;
    }

    LOG_VERBOSE(LogChunkValidation, "Spawned on a non locally controlled client (Pawn: %s). Deactivating.", *OWNING_PAWN->GetName())

    this->SetComponentTickEnabled(false);
    this->ChunkGeneratorSubsystem = nullptr;
    this->WorldGeneratorInfo = nullptr;
    this->ChunkWorldSettings = nullptr;

    return;
}

void ULocalChunkValidator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    this->bExecutePreHyperlaneValidationFuture = false;

    if (this->PreHyperlaneValidationFuture.IsValid())
    {
        if (UNetworkStatics::IsSafeServer(this))
        {
            LOG_ERROR(LogChunkValidation, "Found pre hyperlane validation future on server. Something fishy is going on.")
        }

        LOG_VERBOSE(LogChunkValidation, "Waiting for Pre Hyperlane Validation future to finish.")
        this->PreHyperlaneValidationFuture.Wait();
        LOG_VERBOSE(LogChunkValidation, "Pre Hyperlane Validation future finished.")
    }

    return;
}

void ULocalChunkValidator::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    LOG_VERY_VERBOSE(LogChunkValidation, "Validating local chunks.")

    if (UNetworkStatics::IsSafeClient(this))
    {
        this->GenerateMockChunksOnClient();
    }
    else
    {
        this->GenerateMockChunks();
    }

    return;
}

void ULocalChunkValidator::CreatePreHyperlaneValidationFuture(void)
{
    if (this->PreHyperlaneValidationFuture.IsValid())
    {
        LOG_FATAL(LogChunkValidation, "Future already exists.")
        return;
    }

    check(this->GetOwner())
    check( Cast<APawn>(this->GetOwner()) )
    check( Cast<APawn>(this->GetOwner())->GetController() )
    AWorldPlayerController* OwningController = Cast<AWorldPlayerController>(Cast<APawn>(this->GetOwner())->GetController());
    check( OwningController )

    this->PreHyperlaneValidationFuture = Async(EAsyncExecution::Thread, [this, OwningController] ()
    {
        check( OwningController )

        const double StartTime = FPlatformTime::Seconds();
        while (this->bExecutePreHyperlaneValidationFuture && OwningController->IsConnectionValidAndEstablished() == false)
        {
            if (FPlatformTime::Seconds() - StartTime > Hyperlane::UnverifiedHyperlaneConnectionExhaustionTimeInSeconds)
            {
                break;
            }

            FPlatformProcess::Sleep(0.1f);

            continue;
        }

        if (this->bExecutePreHyperlaneValidationFuture == false)
        {
            LOG_DISPLAY(LogChunkValidation, "Chunk validation future was cancelled.")
            return;
        }

        LOG_DISPLAY(LogChunkValidation, "Chunk validation future will be exectued next tick if still valid.")

        AsyncTask(ENamedThreads::GameThread, [this, OwningController] ()
        {
            if (this->bExecutePreHyperlaneValidationFuture == false)
            {
                LOG_DISPLAY(LogChunkValidation, "Chunk validation future was cancelled in the end of the last tick.")
                return;
            }

            this->bExecutePreHyperlaneValidationFuture = false;

            check( OwningController )

            if (OwningController->IsConnectionValidAndEstablished() == false)
            {
                LOG_FATAL(
                    LogChunkValidation,
                    "Hyperlane connection is still not established or validated after the future exhaustion time has been reached."
                )
                return;
            }

            while (this->PreHyperlaneValidationChunkKey.IsEmpty() == false)
            {
                FIntVector Key;
                this->PreHyperlaneValidationChunkKey.Dequeue(Key);
                this->AskServerToSpawnChunk_ServerRPC(Key);
            }

            LOG_DISPLAY(LogChunkValidation, "Future pushed successfully all pending chunk requests.")

            return;
        });

        return;
    });

    return;
}

bool ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC_Validate(const FIntVector& ChunkKey)
{
    // Do we want to check here if the hyperlane is already validated?

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

    if (ACommonChunk* Chunk = nullptr; this->WorldGeneratorInfo->HasFullyLoadedChunk(ChunkKey, Chunk))
    {
        LOG_VERY_VERBOSE(
            LogChunkValidation,
            "Chunk %s already loaded. Directly sending authority data to asking client.",
            *ChunkKey.ToString()
        )

        check( Chunk )

        CHECK_OWNING_CONTROLLER
        Chunk->SendInitializationDataToClient(OWNING_CONTROLLER);

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

        if ((UNetworkStatics::IsSafeListenServer(this) || UNetworkStatics::IsSafeStandalone(this)) && OWNING_PAWN->IsLocallyControlled())
        {
            LOG_VERY_VERBOSE(
                LogChunkValidation,
                "Delegate cleanup done for %s. Discarding authority data transmission as this chunk is already loaded. Reason: %s.",
                *ChunkKey.ToString(), UNetworkStatics::IsSafeListenServer(this) ? TEXT("Listen server") : TEXT("Standalone")
            )
            return;
        }

        LOG_VERY_VERBOSE(
            LogChunkValidation,
            "Chunk %s finished generating. Asking the Hyperlane Transmitter to give the authority data to the asking client.",
            *Chunk->GetChunkKey().ToString()
        )

        CHECK_OWNING_CONTROLLER
        Chunk->SendInitializationDataToClient(OWNING_CONTROLLER);

        return;
    });

    this->ChunkHandles.Add(ChunkKey, Handle);

    if (this->WorldGeneratorInfo->AddChunkToGenerationQueue(ChunkKey) == false)
    {
        LOG_FATAL(LogChunkValidation, "Unknown error while adding item [%s] to chunk generation queue.", *ChunkKey.ToString())
        return;
    }

    LOG_VERY_VERBOSE(LogChunkValidation, "Chunk %s requested. Waiting for generation delegate to be called.", *ChunkKey.ToString())

    return;
}

void ULocalChunkValidator::SafeSpawnChunk(const FIntVector& ChunkKey)
{
    LOG_VERBOSE(LogChunkValidation, "Validator is asking for spawn chunk approval. Key: %s.", *ChunkKey.ToString())

    CHECK_OWNING_PAWN

    if (UNetworkStatics::IsSafeDedicatedServer(this) || OWNING_PAWN->IsLocallyControlled() == false)
    {
        LOG_FATAL(LogChunkValidation, "Disallowed call. SV: %s; LC: %s.",
            UNetworkStatics::IsSafeServer(this) ? TEXT("true") : TEXT("false"),
            OWNING_PAWN->IsLocallyControlled()  ? TEXT("true") : TEXT("false"))
        return;
    }

    if (UNetworkStatics::IsSafeListenServer(this) || UNetworkStatics::IsSafeStandalone(this))
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

    if (UNetworkStatics::IsSafeDedicatedServer(this))
    {
        LOG_FATAL(LogChunkValidation, "Disallowed call on dedicated server.")
        return;
    }

    check( GEngine )
    check( this->GetWorld() )
    check( GEngine->GetFirstGamePlayer(this->GetWorld()) )
    ULocalPlayerChunkGeneratorSubsystem* Subsystem = GEngine->GetFirstGamePlayer(this->GetWorld())->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>();
    check( Subsystem )
    if (Subsystem->LoadedChunks.Contains(ChunkKey))
    {
        LOG_FATAL(LogChunkValidation, "Chunk already loaded.")
        return;
    }

    const FTransform TargetedChunkTransform = FTransform(
            FRotator::ZeroRotator,
            FVector(
                ChunkKey.X * ChunkWorldSettings::ChunkSize * ChunkWorldSettings::JToUScale,
                ChunkKey.Y * ChunkWorldSettings::ChunkSize * ChunkWorldSettings::JToUScale,
                ChunkKey.Z * ChunkWorldSettings::ChunkSize * ChunkWorldSettings::JToUScale
            ),
            FVector::OneVector
        );
    ACommonChunk* Chunk = this->GetWorld()->SpawnActor<ACommonChunk>(AGreedyChunk::StaticClass(), TargetedChunkTransform);
    check( Chunk )
    Subsystem->LoadedChunks.Add(ChunkKey, Chunk);
    LOG_VERY_VERBOSE(LogChunkValidation, "Loaded empty chunk %s into UWorld.", *ChunkKey.ToString())

    check(this->GetOwner())
    check( Cast<APawn>(this->GetOwner()) )
    check( Cast<APawn>(this->GetOwner())->GetController() )
    check( Cast<AWorldPlayerController>(Cast<APawn>(this->GetOwner())->GetController()) )
    if (Cast<AWorldPlayerController>(Cast<APawn>(this->GetOwner())->GetController())->IsConnectionValidAndEstablished() == false)
    {
        this->PreHyperlaneValidationChunkKey.Enqueue(ChunkKey);

        if (this->PreHyperlaneValidationFuture.IsValid())
        {
            return;
        }

        LOG_DISPLAY(
            LogChunkValidation,
            "Hyperlane is yet waiting to be approved. Creating future and enqueuing chunk %s and all future chunk reqeusts until validated.",
            *ChunkKey.ToString()
        )

        this->bExecutePreHyperlaneValidationFuture = true;
        this->CreatePreHyperlaneValidationFuture();

        return;
    }

    this->bExecutePreHyperlaneValidationFuture = false;

    if (this->PreHyperlaneValidationChunkKey.IsEmpty())
    {
        this->AskServerToSpawnChunk_ServerRPC(ChunkKey);
    }

    while (this->PreHyperlaneValidationChunkKey.IsEmpty() == false)
    {
        FIntVector Key;
        this->PreHyperlaneValidationChunkKey.Dequeue(Key);
        this->AskServerToSpawnChunk_ServerRPC(Key);
    }

    this->AskServerToSpawnChunk_ServerRPC(ChunkKey);

    return;
}

void ULocalChunkValidator::GenerateMockChunks(void)
{
    check( this->ChunkWorldSettings )

    if (this->bFinishedMockingChunkGeneration)
    {
        return;
    }

    if (UNetworkStatics::IsSafeDedicatedServer(this))
    {
        LOG_FATAL(LogChunkValidation, "Disallowed call on dedicated server.")
        return;
    }

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

    /* first iteration */
    if (this->MockCursor == 1)
    {
        SpiralsAddedThisTick++;

        for (int Z = this->ChunkWorldSettings->ChunksAboveZero; Z >= 0; --Z)
        {
            const FIntVector Key = FIntVector(0, 0, Z);
            this->SafeSpawnChunk(Key);
        }
    }

    while (true)
    {
        for (int _ = 0; _ < 2; ++_)
        {
            CurrentMoveIndex = (CurrentMoveIndex + 1) % Moves.Num();
            for (int __ = 0; __ < TimesToMove; ++__)
            {
                TargetPoint = Moves[CurrentMoveIndex](TargetPoint);

                ++MockCursor;
                ++SpiralsAddedThisTick;
                for (int Z = this->ChunkWorldSettings->ChunksAboveZero; Z >= 0; --Z)
                {
                    const FIntVector Key = FIntVector(TargetPoint.X, TargetPoint.Y, Z);

                    this->SafeSpawnChunk(Key);
                }


                continue;
            }

            continue;
        }

        ++TimesToMove;

        if (MockCursor >= ChunkWorldSettings->MaxSpiralPoints)
        {
            this->bFinishedMockingChunkGeneration = true;
            return;
        }

        continue;
    }
}

void ULocalChunkValidator::GenerateMockChunksOnClient(void)
{
    if (UNetworkStatics::IsSafeServer(this))
    {
        LOG_FATAL(LogChunkValidation, "Disallowed call on server.")
        return;
    }

    check( this->ChunkWorldSettings == nullptr )

    if (this->bFinishedMockingChunkGeneration)
    {
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

    /* first iteration */
    if (this->MockCursor == 1)
    {
        SpiralsAddedThisTick++;

        for (int Z = ChunksAboveZeroClient; Z >= 0; --Z)
        {
            const FIntVector Key = FIntVector(0, 0, Z);
            this->SafeSpawnChunk(Key);
        }
    }

    while (true)
    {
        for (int _ = 0; _ < 2; ++_)
        {
            CurrentMoveIndex = (CurrentMoveIndex + 1) % Moves.Num();
            for (int __ = 0; __ < TimesToMove; ++__)
            {
                TargetPoint = Moves[CurrentMoveIndex](TargetPoint);

                ++MockCursor;
                ++SpiralsAddedThisTick;
                for (int Z = ChunksAboveZeroClient; Z >= 0; --Z)
                {
                    const FIntVector Key = FIntVector(TargetPoint.X, TargetPoint.Y, Z);

                    this->SafeSpawnChunk(Key);
                }

                continue;
            }

            continue;
        }

        ++TimesToMove;

        if (MockCursor >= MaxSpiralPointsClient)
        {
            this->bFinishedMockingChunkGeneration = true;
            return;
        }

        continue;
    }
}

#undef OWNING_PAWN
#undef CHECK_OWNING_PAWN
#undef CHECKED_OWNING_PAWN
#undef OWNING_CONTROLLER
