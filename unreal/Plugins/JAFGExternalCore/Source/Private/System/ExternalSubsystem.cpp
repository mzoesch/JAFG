// Copyright 2024 mzoesch. All rights reserved.

#include "System/ExternalSubsystem.h"

#include "Internal/ExternalCoreInternalSettings.h"

UExternalSubsystem::UExternalSubsystem() : Super()
{
    return;
}

void UExternalSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency(GetDefault<UExternalCoreInternalSettings>()->PreInternalInitializationSubsystem);
    Super::Initialize(Collection);

    return;
}
