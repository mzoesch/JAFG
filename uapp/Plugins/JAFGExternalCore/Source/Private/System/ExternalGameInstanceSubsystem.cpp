// Copyright 2024 mzoesch. All rights reserved.

#include "System/ExternalGameInstanceSubsystem.h"
#include "JAFGExternalCore.h"
#include "JAFGLogDefs.h"

UExternalGameInstanceSubsystem::UExternalGameInstanceSubsystem() : Super()
{
    return;
}

void UExternalGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
#if !UE_BUILD_SHIPPING
    if (
           FJAFGExternalCoreModule::Get().bInternalSubsystemInitialized == false
        && FJAFGExternalCoreModule::Get().OnInitInternalSubsystem.IsBound() == false
        )
    {
        LOG_FATAL(LogModSubsystem, "OnInitInternalSubsystem delegate not bound and was not executed beforehand.")
    }
#endif /* !UE_BUILD_SHIPPING */

    FJAFGExternalCoreModule::Get().OnInitInternalSubsystem.ExecuteIfBound(Collection);

    Super::Initialize(Collection);

    return;
}
