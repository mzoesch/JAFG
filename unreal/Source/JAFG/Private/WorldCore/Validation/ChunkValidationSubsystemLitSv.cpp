// Copyright 2024 mzoesch. All rights reserved.

#include "WorldCore/Validation/ChunkValidationSubsystemLitSv.h"

#include "Editor.h"
#include "LevelEditorViewport.h"
#include "WorldCore/ChunkWorldSettings.h"
#include "WorldCore/Chunk/ChunkGenerationSubsystem.h"
#include "WorldCore/Validation/ChunkValidationSubsystemCl.h"
#include "WorldCore/Validation/ChunkValidationSubsystemDedSv.h"
#include "WorldCore/Validation/ChunkValidationSubsystemStandalone.h"

UChunkValidationSubsystemLitSv::UChunkValidationSubsystemLitSv() : Super()
{
    return;
}

void UChunkValidationSubsystemLitSv::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<ULocalChunkWorldSettings>();
    Collection.InitializeDependency<UChunkGenerationSubsystem>();
    Super::Initialize(Collection);

    this->SetTickInterval(2.0f);

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
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystems: Cl.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemDedSv>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystems: DedSv.")
    }
    if (this->GetWorld()->GetSubsystem<UChunkValidationSubsystemStandalone>() != nullptr)
    {
        LOG_FATAL(LogChunkValidation, "Found other validation subsystem. Disallowed. Faulty subsystems: Standalone.")
    }

    this->ChunkGenerationSubsystem = this->GetWorld()->GetSubsystem<UChunkGenerationSubsystem>();
    check( this->ChunkGenerationSubsystem )

    return;
}

void UChunkValidationSubsystemLitSv::MyTick(const float DeltaTime)
{
    Super::MyTick(DeltaTime);

    FVector LocalPlayerLocation;
#if WITH_EDITOR
    if (GEditor->bIsSimulatingInEditor)
    {
        LocalPlayerLocation = GCurrentLevelEditingViewportClient->ViewTransformPerspective.GetLocation();
    }
    else
    {
        LocalPlayerLocation = GEngine->GetFirstLocalPlayerController(this->GetWorld())->GetPawnOrSpectator()->GetActorLocation();
    }
#else /* WITH_EDITOR */
    LocalPlayerLocation = GEngine->GetFirstLocalPlayerController(this->GetWorld())->GetPawnOrSpectator()->GetActorLocation();
#endif /* !WITH_EDITOR */

    this->LoadUnLoadMyChunks(LocalPlayerLocation);

    this->LoadUnloadTheirChunks();

    return;
}

void UChunkValidationSubsystemLitSv::LoadUnLoadMyChunks(const FVector& LocalPlayerLocation) const
{
}

void UChunkValidationSubsystemLitSv::LoadUnloadTheirChunks(void) const
{
}
