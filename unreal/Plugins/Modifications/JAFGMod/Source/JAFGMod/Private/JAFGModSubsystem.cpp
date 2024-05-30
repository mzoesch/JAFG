// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGModSubsystem.h"

#include "JAFGLogDefs.h"
#include "ModificationSupervisorSubsystem.h"

void UJAFGModSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Warning, TEXT("JAFGModSubsystem Initialized: the mod."))

    UModificationSupervisorSubsystem* ModSupervisor = this->GetGameInstance()->GetSubsystem<UModificationSupervisorSubsystem>();

    check( ModSupervisor )

    ModSupervisor->InitializeOptionalVoxelsEvent.AddLambda( [this] { this->OnOptionalVoxelsInitialize(); });

    return;
}

void UJAFGModSubsystem::OnOptionalVoxelsInitialize(void)
{
    LOG_WARNING(LogModSubsystem, "Initializing optional voxels.")
}
