// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGTestModTwo.h"
#include "JAFGLogDefs.h"
#include "JAFGTestModTwoSubsystem.h"
#include "ModificationSupervisorSubsystem.h"

#define LOCTEXT_NAMESPACE "FJAFGTestModTwoModule"

void FJAFGTestModTwoModule::StartupModule(void)
{
    LOG_DISPLAY(LogModSubsystem, "JAFGTestModTwo plugin loaded. Adding self to dependencies.")
    UModificationSupervisorSubsystem::AddMod(UJAFGTestModTwoSubsystem::StaticClass());
    return;
}

void FJAFGTestModTwoModule::ShutdownModule(void)
{
    return;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJAFGTestModTwoModule, JAFGTestModTwo)
