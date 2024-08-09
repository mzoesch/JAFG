// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationSupervisorSubsystem.h"

#include "JAFGLogDefs.h"

UModificationSupervisorSubsystem::UModificationSupervisorSubsystem(void) : Super()
{
    return;
}

void UModificationSupervisorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
#if !WITH_EDITOR
    checkNoReentry()
#endif /* !WITH_EDITOR */

    LOG_DISPLAY(LogModSubsystem, "Called. Initializing external mod subsystems.")

    for (
          TSubclassOf<UExternalModificationSubsystem> ModSubsystem
        : UModificationSupervisorSubsystem::ModSubsystems
    )
    {
        Collection.InitializeDependency(ModSubsystem);
    }

    LOG_DISPLAY(LogModSubsystem, "Finished initializing external mod subsystems. Found %d mods.", UModificationSupervisorSubsystem::ModSubsystems.Num())

    Super::Initialize(Collection);

    return;
}

void UModificationSupervisorSubsystem::AddMod(const TSubclassOf<UExternalModificationSubsystem> ModSubsystem)
{
    if (UModificationSupervisorSubsystem::ModSubsystems.Contains(ModSubsystem))
    {
        LOG_FATAL(LogModSubsystem, "Mod [%s] already exists.", *ModSubsystem->GetName())
        return;
    }

    UModificationSupervisorSubsystem::ModSubsystems.Add(ModSubsystem);

    return;
}
