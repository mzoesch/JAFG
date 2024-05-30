// Copyright 2024 mzoesch. All rights reserved.

#include "ModificationSupervisorSubsystem.h"

#include "JAFGLogDefs.h"

UModificationSupervisorSubsystem::UModificationSupervisorSubsystem()
{
}

void UModificationSupervisorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    LOG_WARNING(LogModSubsystem, "Called. Initializing external mod subsystems." )

    for (
          TSubclassOf<UExternalModificationSubsystem> ModSubsystem
        : UModificationSupervisorSubsystem::ModSubsystems
    )
    {
        Collection.InitializeDependency(ModSubsystem);
    }

    this->PrintMods();

    Super::Initialize(Collection);

    return;
}

void UModificationSupervisorSubsystem::AddMod(const TSubclassOf<UExternalModificationSubsystem> ModSubsystem)
{
    if (UModificationSupervisorSubsystem::ModSubsystems.Contains(ModSubsystem))
    {
        LOG_FATAL(LogTemp, "Mod [%s] already exists.", *ModSubsystem->GetName())
        return;
    }

    UModificationSupervisorSubsystem::ModSubsystems.Add(ModSubsystem);

    return;
}

void UModificationSupervisorSubsystem::PrintMods(void)
{
    UE_LOG(LogTemp, Warning, TEXT("Mods classes found: %d."), UModificationSupervisorSubsystem::ModSubsystems.Num())
}
