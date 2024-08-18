// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemCl.h"
#include "Network/MyHyperlaneComponent.h"
#include "Player/WorldPlayerController.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#include "WorldCore/Validation/ChunkValidationSubsystemDedSv.h"
#include "WorldCore/Validation/ChunkValidationSubsystemLitSv.h"
#include "WorldCore/Validation/ChunkValidationSubsystemStandalone.h"

UChunkValidationSubsystemCl::UChunkValidationSubsystemCl() : Super()
{
    return;
}

bool UChunkValidationSubsystemCl::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return UNetStatics::IsSafeClient(Outer);
}

void UChunkValidationSubsystemCl::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemDedSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: DedSv.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemLitSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: LitSv.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemStandalone>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: Standalone.")
    }

    return;
}

void UChunkValidationSubsystemCl::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    const AWorldPlayerController* LocalController = this->GetLocalPlayerController<AWorldPlayerController>();

    if (LocalController == nullptr)
    {
        return;
    }

    if (LocalController->GetComponentByClass<UMyHyperlaneComponent>()->IsConnectedAndReady() == false)
    {
        return;
    }

    if (this->ChunkGenerationSubsystem->IsReady() == false)
    {
        return;
    }

    FVector PredictedLocation;
    if (this->GetPredictedLocalPlayerLocation(PredictedLocation) == false)
    {
        return;
    }

    FChunkLoadingParams Params; Params.RenderDistance = 5;
    this->LoadUnloadChunks({PredictedLocation}, Params);

    /* Already sent request. */
    if (LocalController->IsClientReadyForCharacterSpawn())
    {
        return;
    }
    this->TrySpawnLocalCharacter();

    return;
}
