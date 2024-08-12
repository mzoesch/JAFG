// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGTestModFive.h"
#include "JAFGLogDefs.h"
#include "JAFGTestModFiveSubsystem.h"
#include "ModificationSupervisorSubsystem.h"

#define LOCTEXT_NAMESPACE "FJAFGTestModFiveModule"

void FJAFGTestModFiveModule::StartupModule(void)
{
    LOG_DISPLAY(LogModSubsystem, "JAFGTestModFive plugin loaded. Adding self to dependencies.")
    UModificationSupervisorSubsystem::AddMod(UJAFGTestModFiveSubsystem::StaticClass());
    return;
}

void FJAFGTestModFiveModule::ShutdownModule(void)
{
    return;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJAFGTestModFiveModule, JAFGTestModFive)
