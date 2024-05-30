// Copyright 2024 mzoesch. All rights reserved.

#include "ExternalModificationSubsystem.h"

#include "ModificationSupervisorSubsystem.h"

void UExternalModificationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UModificationSupervisorSubsystem>();
    Super::Initialize(Collection);
}
