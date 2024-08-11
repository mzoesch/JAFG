// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGTestModOneSubsystem.h"
#include "JAFGLogDefs.h"

void UJAFGTestModOneSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LOG_DISPLAY(LogModSubsystem, "Called.")

    return;
}
