// Copyright 2024 mzoesch. All rights reserved.

#include "System/PreInternalInitializationSubsystem.h"

#include "System/JAFGGameInstance.h"
#include "ModificationSupervisorSubsystem.h"

void UPreInternalInitializationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    if (this->GetTemplateGameInstance<UJAFGGameInstance>()->bInitializedExternalMods)
    {
        Super::Initialize(Collection);
        return;
    }
    this->GetTemplateGameInstance<UJAFGGameInstance>()->bInitializedExternalMods = true;

    Collection.InitializeDependency(UModificationSupervisorSubsystem::StaticClass());

    Super::Initialize(Collection);

    return;
}

void UPreInternalInitializationSubsystem::InitializeOptionalVoxels(TArray<FVoxelMask>& VoxelMasks)
{
    this->GetGameInstance()->GetSubsystem<UModificationSupervisorSubsystem>()->InitializeOptionalVoxelsEvent.Broadcast(VoxelMasks);
}
