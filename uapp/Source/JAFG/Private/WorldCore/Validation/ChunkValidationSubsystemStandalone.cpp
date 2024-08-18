// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemStandalone.h"
#include "JAFGSettingsLocal.h"
#include "Player/JAFGLocalPlayer.h"
#include "Player/WorldPlayerController.h"
#include "WorldCore/Validation/ChunkValidationSubsystemCl.h"
#include "WorldCore/Validation/ChunkValidationSubsystemDedSv.h"
#include "WorldCore/Validation/ChunkValidationSubsystemLitSv.h"

UChunkValidationSubsystemStandalone::UChunkValidationSubsystemStandalone() : Super()
{
    return;
}

bool UChunkValidationSubsystemStandalone::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

#if WITH_EDITOR
    return UNetStatics::IsSafeStandalone(Outer);
#else /* WITH_EDITOR */
    return UNetStatics::IsSafeStandaloneNoServer(Outer);
#endif /* !WITH_EDITOR */
}

void UChunkValidationSubsystemStandalone::OnWorldBeginPlay(UWorld& InWorld)
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
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemLitSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystem: LitSv.")
    }

    return;
}

void UChunkValidationSubsystemStandalone::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    FVector PredictedLocation;
    if (this->GetPredictedLocalPlayerLocation(PredictedLocation) == false)
    {
        return;
    }

    FChunkLoadingParams Params;
    Params.RenderDistance = Cast<UJAFGLocalPlayer>(this->GetLocalPlayerController()->GetLocalPlayer())->GetLocalSettings()->GetClientRenderDistance();
    this->LoadUnloadChunks({PredictedLocation}, Params);

    this->TrySpawnLocalCharacter();

    return;
}
