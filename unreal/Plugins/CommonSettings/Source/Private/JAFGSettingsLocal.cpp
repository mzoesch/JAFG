// Copyright 2024 mzoesch. All rights reserved.

#include "JAFGSettingsLocal.h"

#include "JAFGLogDefs.h"

UJAFGSettingsLocal::UJAFGSettingsLocal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    return;
}

UJAFGSettingsLocal* UJAFGSettingsLocal::Get(void)
{
    return GEngine ? CastChecked<UJAFGSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

float UJAFGSettingsLocal::GetMasterVolume(void) const
{
    return this->MasterVolume;
}

void UJAFGSettingsLocal::SetMasterVolume(const float InMasterVolume)
{
    this->MasterVolume = InMasterVolume;
}

const TArray<FLoadedInputAction>& UJAFGSettingsLocal::GetAllLoadedInputActions(void) const
{
    if (this->OwningInputSubsystem == nullptr)
    {
        LOG_FATAL(LogGameSettings, "Owning input subsystem is not set.")
        static const TArray<FLoadedInputAction> Out = TArray<FLoadedInputAction>();
        return Out;
    }

    return this->OwningInputSubsystem->GetAllLoadedInputActions();
}

FColor UJAFGSettingsLocal::GetPrimaryColor(void) const
{
    return this->PrimaryColor;
}

void UJAFGSettingsLocal::SetPrimaryColor(const FColor InPrimaryColor)
{
    this->PrimaryColor = InPrimaryColor;
}
