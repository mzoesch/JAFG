// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2024 mzoesch. All rights reserved.

#include "PLUGIN_NAME.h"
#include "JAFGLogDefs.h"
#include "PLUGIN_NAMESubsystem.h"
#include "ModificationSupervisorSubsystem.h"

#define LOCTEXT_NAMESPACE "FPLUGIN_NAMEModule"

void FPLUGIN_NAMEModule::StartupModule(void)
{
    LOG_DISPLAY(LogModSubsystem, "PLUGIN_NAME plugin loaded. Adding self to dependencies.")
    UModificationSupervisorSubsystem::AddMod(UPLUGIN_NAMESubsystem::StaticClass());
    return;
}

void FPLUGIN_NAMEModule::ShutdownModule(void)
{
    return;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPLUGIN_NAMEModule, PLUGIN_NAME)
