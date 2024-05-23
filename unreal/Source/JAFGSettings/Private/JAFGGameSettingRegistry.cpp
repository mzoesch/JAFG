// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGGameSettingRegistry.h"

#include "JAFGLogDefs.h"

UJAFGGameSettingRegistrySubsystem::UJAFGGameSettingRegistrySubsystem(void) : Super()
{
    return;
}

void UJAFGGameSettingRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    this->LocalRegistry = NewObject<UJAFGGameSettingRegistry>(); check( this->LocalRegistry )
    this->LocalRegistry->Initialize(this->GetLocalPlayer());

    LOG_VERBOSE(LogGameSettings, "Initialzied local registry.")

    return;
}

UJAFGGameSettingRegistry::UJAFGGameSettingRegistry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

void UJAFGGameSettingRegistry::Initialize(ULocalPlayer* InLocalPlayer)
{
    Super::Initialize(InLocalPlayer);
}
