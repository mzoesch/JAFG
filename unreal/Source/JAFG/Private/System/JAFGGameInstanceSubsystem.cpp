// Copyright 2024 mzoesch. All rights reserved.

#include "System/JAFGGameInstanceSubsystem.h"

#include "ModificationSupervisorSubsystem.h"
#include "System/JAFGGameInstance.h"

void UJAFGGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
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
