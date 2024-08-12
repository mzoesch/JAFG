// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGTestModFour.h"
#include "JAFGLogDefs.h"
#include "JAFGTestModFourSubsystem.h"
#include "ModificationSupervisorSubsystem.h"

#define LOCTEXT_NAMESPACE "FJAFGTestModFourModule"

void FJAFGTestModFourModule::StartupModule(void)
{
    LOG_DISPLAY(LogModSubsystem, "JAFGTestModFour plugin loaded. Adding self to dependencies.")
    UModificationSupervisorSubsystem::AddMod(UJAFGTestModFourSubsystem::StaticClass());
    return;
}

void FJAFGTestModFourModule::ShutdownModule(void)
{
    return;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJAFGTestModFourModule, JAFGTestModFour)
