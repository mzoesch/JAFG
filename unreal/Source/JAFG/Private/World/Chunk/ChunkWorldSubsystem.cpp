// Copyright 2024 mzoesch. All rights reserved.

#include "World/Chunk/ChunkWorldSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Network/ChunkMulticasterInfo.h"
#include "Network/NetworkStatics.h"

bool UChunkWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    check( Outer )

    const bool bSuperCreate = Super::ShouldCreateSubsystem(Outer);

    LOG_VERBOSE(LogChunkMisc, "Called.")

    if (bSuperCreate == false)
    {
        return false;
    }

    check( Outer->GetWorld() )

    if (Outer->GetWorld()->GetName() == TEXT("L_World"))
    {
        return true;
    }

    LOG_WARNING(LogChunkMisc, "Subsystem will not be created for world %s.", *Outer->GetWorld()->GetName())

    return false;
}

void UChunkWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LOG_VERBOSE(LogChunkMisc, "Called.")
}

void UChunkWorldSubsystem::PostInitialize(void)
{
    Super::PostInitialize();
    LOG_VERBOSE(LogChunkMisc, "Called.")
}

void UChunkWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);
    LOG_VERBOSE(LogChunkMisc, "Called.")

    if (UNetworkStatics::IsSafeClient(this))
    {
        LOG_DISPLAY(LogChunkMisc, "Not initializing Chunk World Settings on client.")
        return;
    }

    LOG_DISPLAY(LogChunkMisc, "Initializing Chunk World Settings based on current server settings.")

    this->ChunkMulticasterInfo = Cast<AChunkMulticasterInfo>(UGameplayStatics::GetActorOfClass(this, AChunkMulticasterInfo::StaticClass()));
    check( this->ChunkMulticasterInfo )

    check( this->ChunkWorldSettings == nullptr )
    check( this->GetWorld() )
    check( this->GetWorld()->GetWorldSettings() )
    this->ChunkWorldSettings = Cast<AChunkWorldSettings>(this->GetWorld()->GetWorldSettings());
    check( this->ChunkWorldSettings )

    if (this->ChunkWorldSettings->ContinentalnessSpline.Num() < 2)
    {
        LOG_FATAL(LogChunkMisc, "Continentalness spline must have at least 2 points.")
        return;
    }

    /* Transform Kismet types to C++. */
    this->ChunkWorldSettings->WorldNoiseType   = ENoiseType::KismetToCPP(this->ChunkWorldSettings->WorldKismetNoiseType);
    this->ChunkWorldSettings->WorldFractalType = EFractalType::KismetToCPP(this->ChunkWorldSettings->WorldKismetFractalType);
    this->ChunkWorldSettings->ContinentalnessNoiseType   = ENoiseType::KismetToCPP(this->ChunkWorldSettings->ContinentalnessKismetNoiseType);
    this->ChunkWorldSettings->ContinentalnessFractalType = EFractalType::KismetToCPP(this->ChunkWorldSettings->ContinentalnessKismetFractalType);

    this->ChunkWorldSettings->NoiseWorld = new FFastNoiseLite();
    this->ChunkWorldSettings->NoiseWorld->SetSeed(this->ChunkWorldSettings->Seed);
    this->ChunkWorldSettings->NoiseWorld->SetFrequency(this->ChunkWorldSettings->WorldFrequency);
    this->ChunkWorldSettings->NoiseWorld->SetFractalType(this->ChunkWorldSettings->WorldFractalType);

    this->ChunkWorldSettings->NoiseContinentalness = new FFastNoiseLite();
    this->ChunkWorldSettings->NoiseContinentalness->SetSeed(this->ChunkWorldSettings->Seed);
    this->ChunkWorldSettings->NoiseContinentalness->SetFrequency(this->ChunkWorldSettings->ContinentalnessFrequency);
    this->ChunkWorldSettings->NoiseContinentalness->SetFractalType(this->ChunkWorldSettings->ContinentalnessFractalType);

    return;
}

void UChunkWorldSubsystem::Deinitialize(void)
{
    Super::Deinitialize();
    LOG_VERBOSE(LogChunkMisc, "Called.")

    if (UNetworkStatics::IsSafeClient(this))
    {
        /*
         * Settings are only valid on the server.
         */
        return;
    }

    if (this->ChunkWorldSettings)
    {
        delete this->ChunkWorldSettings->NoiseWorld;
        delete this->ChunkWorldSettings->NoiseContinentalness;
    }

    return;
}

void UChunkWorldSubsystem::BroadcastChunkModification(const FIntVector& ChunkKey, const FIntVector& LocalVoxelLocation, const int32 Voxel) const
{
    if (UNetworkStatics::IsSafeClient(this) || UNetworkStatics::IsSafeStandalone(this))
    {
        LOG_FATAL(LogChunkMisc, "Disallowed. Reason: %s", UNetworkStatics::IsSafeClient(this) ? *FString("Client") : *FString("Standalone"))
        return;
    }

    LOG_VERY_VERBOSE(
        LogChunkMisc,
        "Broadcasting chunk modification to all clients: CK: %s, LV: %s, V: %d",
        *ChunkKey.ToString(), *LocalVoxelLocation.ToString(), Voxel
    )

    this->ChunkMulticasterInfo->MulticastChunkModification(ChunkKey, LocalVoxelLocation, Voxel);

    return;
}
