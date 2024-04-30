// Copyright 2024 mzoesch. All rights reserved.

#include "Registry/RegistrySubsystem.h"

#include "Registry/SettingRegistry.h"

void URegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    this->SettingRegistry = NewObject<USettingRegistry>(this);
    this->SettingRegistry->OnInitialize();

    return;
}

void URegistrySubsystem::Deinitialize(void)
{
    Super::Deinitialize();

}
