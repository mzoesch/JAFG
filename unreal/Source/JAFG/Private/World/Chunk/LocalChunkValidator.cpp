// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/LocalChunkValidator.h"

#include "Kismet/GameplayStatics.h"
#include "Network/NetworkStatics.h"
#include "System/LocalPlayerChunkGeneratorSubsystem.h"
#include "World/WorldGeneratorInfo.h"
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

    check( GEngine )
    check( this->GetWorld() )
    ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(this->GetWorld());
    check( LocalPlayer )
    ULocalPlayerChunkGeneratorSubsystem* ChunkGeneratorSubsystem = LocalPlayer->GetSubsystem<ULocalPlayerChunkGeneratorSubsystem>();
    check( ChunkGeneratorSubsystem )


    const FIntVector Key = FIntVector::ZeroValue;
    if (ChunkGeneratorSubsystem->LoadedChunks.Contains(Key))
    {
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

    ChunkGeneratorSubsystem->LoadedChunks.Add(Key, Chunk);

    this->AskServerToSpawnChunk_ServerRPC(Key);

    return;
}

void ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC_Implementation(const FIntVector& ChunkKey)
{
    AActor* _ = UGameplayStatics::GetActorOfClass(this, AWorldGeneratorInfo::StaticClass());
    check( _ )
    AWorldGeneratorInfo* WorldGeneratorInfo = Cast<AWorldGeneratorInfo>(_);
    check( WorldGeneratorInfo )

    if (this->ChunkHandles.Contains(ChunkKey))
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Chunk already requested by client. Not implemented yet."))
        return;
    }

    if (WorldGeneratorInfo->HasFullyLoadedChunk(ChunkKey))
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Chunk already loaded. Not implemented yet.s"))
        return;
    }

    FDelegateHandle Handle = WorldGeneratorInfo->OnChunkFinishedGeneratingDelegate.AddLambda( [&, WorldGeneratorInfo, ChunkKey] (const ACommonChunk* Chunk)
    {
        if (ChunkKey != Chunk->GetChunkKey())
        {
            UE_LOG(LogTemp, Error, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Chunk key mismatch."))
            return;
        }

        UE_LOG(LogTemp, Log, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Chunk %s finished generating. Asking the Hyperlane Transmitter to give the authority data to the asking client."), *Chunk->GetChunkKey().ToString())

        if (this->ChunkHandles.Contains(ChunkKey) == false)
        {
            UE_LOG(LogTemp, Fatal, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Chunk handle not found."))
            return;
        }

        WorldGeneratorInfo->OnChunkFinishedGeneratingDelegate.Remove(*this->ChunkHandles[ChunkKey]);
        this->ChunkHandles.Remove(ChunkKey);

        return;
    });

    this->ChunkHandles.Add(ChunkKey, &Handle);

    if (WorldGeneratorInfo->AddChunkToGenerationQueue(ChunkKey) == false)
    {
        UE_LOG(LogTemp, Fatal, TEXT("ULocalChunkValidator::AskServerToSpawnChunk_ServerRPC: Error while adding item to queue.."))
        return;
    }

    return;
}
