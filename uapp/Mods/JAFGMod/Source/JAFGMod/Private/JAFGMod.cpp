// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGMod.h"
#include "JAFGLogDefs.h"
#include "JAFGModSubsystem.h"
#include "ModificationSupervisorSubsystem.h"

#define LOCTEXT_NAMESPACE "FJAFGModModule"

void FJAFGModModule::StartupModule(void)
{
    LOG_DISPLAY(LogModSubsystem, "JAFGMod Plugin loaded. Adding self to dependencies.")
    UModificationSupervisorSubsystem::AddMod(UJAFGModSubsystem::StaticClass());
    return;
}

void FJAFGModModule::ShutdownModule(void)
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJAFGModModule, JAFGMod)
