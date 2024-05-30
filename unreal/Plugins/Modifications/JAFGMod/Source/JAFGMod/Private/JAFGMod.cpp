// Copyright Epic Games, Inc. All Rights Reserved.

#include "JAFGMod.h"

#include "JAFGModSubsystem.h"
#include "ModificationSupervisorSubsystem.h"

#define LOCTEXT_NAMESPACE "FJAFGModModule"

void FJAFGModModule::StartupModule(void)
{
    UE_LOG(LogTemp, Warning, TEXT("JAFGMod Plugin Loaded!"))

   UModificationSupervisorSubsystem::AddMod(UJAFGModSubsystem::StaticClass());

    return;
}

void FJAFGModModule::ShutdownModule(void)
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJAFGModModule, JAFGMod)
