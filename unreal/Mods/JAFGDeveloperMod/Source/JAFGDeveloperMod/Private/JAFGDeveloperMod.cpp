// Copyright Epic Games, Inc. All Rights Reserved.

#include "JAFGDeveloperMod.h"
#include "JAFGLogDefs.h"
#include "JAFGDeveloperModSubsystem.h"
#include "ModificationSupervisorSubsystem.h"

#define LOCTEXT_NAMESPACE "FJAFGDeveloperModModule"

void FJAFGDeveloperModModule::StartupModule(void)
{
    LOG_DISPLAY(LogModSubsystem, "JAFGDeveloperMod Plugin loaded. Adding self to dependencies.")
    UModificationSupervisorSubsystem::AddMod(UJAFGDeveloperModSubsystem::StaticClass());
    return;
}

void FJAFGDeveloperModModule::ShutdownModule(void)
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJAFGDeveloperModModule, JAFGDeveloperMod)
