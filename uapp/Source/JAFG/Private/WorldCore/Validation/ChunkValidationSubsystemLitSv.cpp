// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemLitSv.h"
#include "Player/WorldPlayerController.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#include "WorldCore/Validation/ChunkValidationSubsystemCl.h"
#include "WorldCore/Validation/ChunkValidationSubsystemDedSv.h"
#include "WorldCore/Validation/ChunkValidationSubsystemStandalone.h"

UChunkValidationSubsystemLitSv::UChunkValidationSubsystemLitSv() : Super()
{
    return;
}

bool UChunkValidationSubsystemLitSv::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return UNetStatics::IsSafeListenServer(Outer);
}

void UChunkValidationSubsystemLitSv::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemCl>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: Cl.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemDedSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: DedSv.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemStandalone>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: Standalone.")
    }

    return;
}

void UChunkValidationSubsystemLitSv::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    FChunkLoadingParams Params; Params.RenderDistance = 5;
    this->LoadUnloadChunks(this->GetAllPredictedPlayerLocations(), Params);

    this->TrySpawnRemoteCharacters();

    return;
}
