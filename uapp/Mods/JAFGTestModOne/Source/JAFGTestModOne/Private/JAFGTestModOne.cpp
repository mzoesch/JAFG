// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGTestModOne.h"
#include "JAFGLogDefs.h"
#include "JAFGTestModOneSubsystem.h"
#include "ModificationSupervisorSubsystem.h"

#define LOCTEXT_NAMESPACE "FJAFGTestModOneModule"

void FJAFGTestModOneModule::StartupModule(void)
{
    LOG_DISPLAY(LogModSubsystem, "JAFGTestModOne plugin loaded. Adding self to dependencies.")
    UModificationSupervisorSubsystem::AddMod(UJAFGTestModOneSubsystem::StaticClass());
    return;
}

void FJAFGTestModOneModule::ShutdownModule(void)
{
    return;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJAFGTestModOneModule, JAFGTestModOne)
