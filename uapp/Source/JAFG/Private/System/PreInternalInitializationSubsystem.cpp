// Copyright 2024 mzoesch. All rights reserved.

#include "System/PreInternalInitializationSubsystem.h"
#include "System/JAFGGameInstance.h"
#include "ModificationSupervisorSubsystem.h"

void UPreInternalInitializationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    if (this->GetTemplateGameInstance<UJAFGGameInstance>()->HasInitializedExternalGamePlugins())
    {
        Super::Initialize(Collection);
        return;
    }
    this->GetTemplateGameInstance<UJAFGGameInstance>()->SetInitializedExternalGamePlugins();

    Collection.InitializeDependency<UModificationSupervisorSubsystem>();

    Super::Initialize(Collection);

    return;
}

void UPreInternalInitializationSubsystem::InitializeOptionalVoxels(TArray<FVoxelMask>& VoxelMasks)
{
    this->GetGameInstance()->GetSubsystem<UModificationSupervisorSubsystem>()->InitializeOptionalVoxelsDelegate.Broadcast(VoxelMasks);
}

void UPreInternalInitializationSubsystem::InitializeOptionalItems(TArray<FItemMask>& VoxelMasks)
{
    this->GetGameInstance()->GetSubsystem<UModificationSupervisorSubsystem>()->InitializeOptionalItemsDelegate.Broadcast(VoxelMasks);
}
