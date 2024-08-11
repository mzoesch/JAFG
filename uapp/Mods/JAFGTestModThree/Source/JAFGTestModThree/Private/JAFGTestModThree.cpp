// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGTestModThree.h"
#include "JAFGLogDefs.h"
#include "JAFGTestModThreeSubsystem.h"
#include "ModificationSupervisorSubsystem.h"

#define LOCTEXT_NAMESPACE "FJAFGTestModThreeModule"

void FJAFGTestModThreeModule::StartupModule(void)
{
    LOG_DISPLAY(LogModSubsystem, "JAFGTestModThree plugin loaded. Adding self to dependencies.")
    UModificationSupervisorSubsystem::AddMod(UJAFGTestModThreeSubsystem::StaticClass());
    return;
}

void FJAFGTestModThreeModule::ShutdownModule(void)
{
    return;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJAFGTestModThreeModule, JAFGTestModThree)
